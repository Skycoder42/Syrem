#include "eventexpressionparser.h"
#include "schedule.h"
#include "terms.h"
#include <chrono>
#include <QtConcurrentRun>
#include <QEventLoop>
#include <QLocale>
#include <QVector>
using namespace Expressions;

EventExpressionParser::EventExpressionParser(QObject *parent) :
	QObject{parent}
{}

MultiTerm EventExpressionParser::parseMultiExpression(const QString &expression)
{
	return parseExpressionImpl(expression, true);
}

TermSelection EventExpressionParser::parseExpression(const QString &expression)
{
	auto resList = parseExpressionImpl(expression, false);
	Q_ASSERT(resList.size() == 1);
	return std::move(resList.first());
}

bool EventExpressionParser::needsSelection(const TermSelection &term) const
{
	return term.size() > 1;
}

bool EventExpressionParser::needsSelection(const MultiTerm &term) const
{
	for(const auto &selTerm : term) {
		if(selTerm.size() > 1)
			return true;
	}

	return false;
}

QSharedPointer<Schedule> EventExpressionParser::createSchedule(const Term &term, const QDateTime &reference)
{
	if(term.isLooped()) {
		Term loop, fence, from, until;
		std::tie(loop, fence, from, until) = term.splitLoop();
		if(!loop.hasTimeScope()) {
			loop.append(QSharedPointer<TimeTerm>::create(_settings->scheduler.defaultTime));
			loop.finalize();
		}

		// get the from date
		QDateTime fromDate;
		if(!from.isEmpty()) {
			if(!from.hasTimeScope()) {
				from.append(QSharedPointer<TimeTerm>::create(QTime{0, 0}));
				from.finalize();
			}
			fromDate = evaluteTerm(from, reference);
		}
		if(!fromDate.isValid())
			fromDate = reference;

		// get the until date
		QDateTime untilDate;
		if(!until.isEmpty()) {
			if(!until.hasTimeScope()) {
				until.append(QSharedPointer<TimeTerm>::create(QTime{0, 0}));
				until.finalize();
			}
			untilDate = evaluteTerm(until, reference);
		}

		if(fromDate.isValid() && untilDate.isValid() && untilDate <= fromDate)
			throw EventExpressionParserException{UntilIsSmallerThenPastError};

		// create schedule and getNext once for the initial date
		auto res = QSharedPointer<RepeatedSchedule>::create(loop, fence, fromDate, untilDate);
		if(res->nextSchedule().isValid())
			return res;
		else
			throw EventExpressionParserException{InitialLoopInvalidError};
	} else
		return QSharedPointer<SingularSchedule>::create(evaluteTerm(term, reference)); // create a "pre-evaluated" one time schedule
}

QSharedPointer<Schedule> EventExpressionParser::createMultiSchedule(MultiTerm terms, const QList<int> &selection, const QDateTime &reference)
{
#ifndef QT_NO_DEBUG
	if(selection.isEmpty()) {
		for(auto i = 0; i < terms.size(); i++) {
			if(terms[i].size() != 1)
				qFatal("MultiTerm without term selection has more then one possibility at index %i", i);
		}
	}
#endif

	if(!selection.isEmpty()) {
		Q_ASSERT(selection.size() == terms.size());
		// reduce the schedule to a single term per expression
		for(auto i = 0; i < terms.size(); i++) {
			auto sel = terms[i].takeAt(selection[i]);
			terms.replace(i, {sel});
		}
	}

	if(terms.size() == 1)
		return createSchedule(terms.first().first(), reference);
	else {
		auto schedule = QSharedPointer<MultiSchedule>::create(reference);
		for(const auto &term : qAsConst(terms))
			schedule->addSubSchedule(createSchedule(term.first(), reference));
		schedule->nextSchedule(); //call to "init" on the first sub schedule
		return schedule;
	}
}

QDateTime EventExpressionParser::evaluteTerm(const Term &term, const QDateTime &reference)
{
	if(term.isLooped())
		throw EventExpressionParserException{TermIsLoopError};

	auto then = term.apply(reference);
	if(!term.hasTimeScope()) {
		QTime time = _settings->scheduler.defaultTime;
		if(time.isValid() && time != QTime{0,0})
			then.setTime(time);
	}
	if(reference < then)
		return then;
	else
		throw EventExpressionParserException{EvaluatesToPastError};
}

MultiTerm EventExpressionParser::parseExpressionImpl(const QString &expression, bool allowMulti)
{
	// prepare eventloop with result signal handlers
	const auto id = QUuid::createUuid();
	MultiTerm terms;
	ErrorInfo lastError;
	QEventLoop loop;
	connect(this, &EventExpressionParser::termCompleted, &loop, [&](QUuid termId, int termIndex, const Term &term){
		if(termId == id)
			terms[termIndex].append(term);
	}, Qt::QueuedConnection);
	connect(this, &EventExpressionParser::errorOccured, &loop, [&](QUuid termId, quint64 significance, EventExpressionParser::ErrorInfo error){
		if(termId == id) {
			QReadLocker lock{&_taskLocker};
			if(_taskCounter[id].second == significance)
				lastError = error;
		}
	}, Qt::QueuedConnection);
	connect(this, &EventExpressionParser::operationCompleted, &loop, [&](QUuid doneId){
		if(doneId == id)
			loop.quit();
	}, Qt::QueuedConnection);

	// start operations
	{
		QWriteLocker lock{&_taskLocker};
		_taskCounter.insert(id, {1, 0});
	}
	if(allowMulti)
		QtConcurrent::run(this, &EventExpressionParser::parseMultiTerm, id, &expression, &terms);
	else {
		terms.append(TermSelection{});
		//parseTerm must be directly called. The manual call to complete is only needed here, as only the async methods do that
		parseTerm(id, &expression, {}, 0, {}, 0);
		completeTask(id);
	}

	auto res = loop.exec();
	{
		QWriteLocker lock{&_taskLocker};
		Q_ASSERT(_taskCounter.value(id).first == 0);
		_taskCounter.remove(id);
	}
	if(res == EXIT_SUCCESS) {
		for(const auto &term : qAsConst(terms)) { // throw error for the first subterm that failed
			if(term.isEmpty()) {
				throw EventExpressionParserException{
					lastError.type,
					lastError.depth,
					lastError.subTermBegin != -1 && lastError.subTermBegin < lastError.depth ?
						expression.midRef(lastError.subTermBegin, lastError.depth - lastError.subTermBegin) :
						QStringRef{}};
			}
		}
		return terms;
	} else
		throw EventExpressionParserException{UnknownError};
}

void EventExpressionParser::parseTerm(QUuid id, const QStringRef &expression, const Term &term, int termIndex, const Term &rootTerm, int depth)
{
	// start parser-tasks for all the possible subterms
	addTasks(id, 10);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<TimeTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<DateTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<InvertedTimeTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<MonthDayTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<WeekDayTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<MonthTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<YearTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<SequenceTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<KeywordTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<LimiterTerm>, TermParams{id, expression, term, termIndex, rootTerm, depth});
}

void EventExpressionParser::validatePartialTerm(const Term &term, int depth)
{
	/* Checks to perform after every subterm:
	 *	1. All Scopes must be unique
	 *	2. Only a single loop is allowed
	 *	3. Only a single span is allowed
	 *	4. Verify there is only a single limiter of each type
	 *	5. Only loops can have limiters
	 */
	SubTerm::Scope allScope = SubTerm::InvalidScope;
	auto hasLoop = false;
	auto hasSpan = false;
	auto hasFromLimiter = false;
	auto hasUntilLimiter = false;
	for(const auto &subTerm : term) {
		if((static_cast<int>(allScope) & static_cast<int>(subTerm->scope)) != 0) // (1)
			throw ErrorInfo{ErrorInfo::SubTermLevel, depth, DuplicateScopeError, -1};
		allScope |= subTerm->scope;

		// (2)
		if(subTerm->type.testFlag(SubTerm::FlagLooped)) {
			if(hasLoop)
				throw ErrorInfo{ErrorInfo::SubTermLevel, depth, DuplicateLoopError, -1};
			else
				hasLoop = true;
		}
		// (3)
		if(subTerm->type.testFlag(SubTerm::Timespan)) {
			if(hasSpan)
				throw ErrorInfo{ErrorInfo::SubTermLevel, depth, DuplicateSpanError, -1};
			else
				hasSpan = true;
		}
		// (4)
		if(subTerm->type == SubTerm::FromSubterm) {
			if(hasFromLimiter)
				throw ErrorInfo{ErrorInfo::SubTermLevel, depth, DuplicateFromLimiterError, -1};
			else
				hasFromLimiter = true;
		}
		if(subTerm->type == SubTerm::UntilSubTerm) {
			if(hasUntilLimiter)
				throw ErrorInfo{ErrorInfo::SubTermLevel, depth, DuplicateUntilLimiterError, -1};
			else
				hasUntilLimiter = true;
		}
	}

	if((hasFromLimiter || hasUntilLimiter) && !hasLoop) // (5)
		throw ErrorInfo{ErrorInfo::SubTermLevel, depth, UnexpectedLimiterError, -1};
}

void EventExpressionParser::validateFullTerm(Term &term, Term &rootTerm, int depth)
{
	/* Checks to perform on the full term:
	 *	1. Sort by scope
	 *	2. Only the first element can be absolute
	 *	3. Timepoints must not be followed by spans, except for loops (as the point part serves as "fence")
	 *
	 * If rootTerm is "valid", aka term is a limiter:
	 *	4. Limiters must not be loops
	 *	5. Verify limiters are "bigger" in scope then the loop fence, if present
	 *	6. Merge the term into root term and swap them
	 */
	std::sort(term.begin(), term.end(), [](const QSharedPointer<SubTerm> &lhs, const QSharedPointer<SubTerm> &rhs){ // (1)
		return lhs->scope > rhs->scope;
	});

	auto isFirst = true;
	auto isPoint = false;
	auto isLoop = false;
	for(const auto &subTerm : qAsConst(term)) {
		// (2)
		if(isFirst)
			isFirst = false;
		else if(subTerm->type.testFlag(SubTerm::FlagAbsolute))
			throw ErrorInfo{ErrorInfo::TermLevel, depth, UnexpectedAbsoluteSubTermError, -1};

		// (3)
		if(!isLoop) {
			if(subTerm->type.testFlag(SubTerm::FlagLooped))
				isLoop = true;
			else if(isPoint && subTerm->type.testFlag(SubTerm::Timespan))
				throw ErrorInfo{ErrorInfo::TermLevel, depth, SpanAfterTimepointError, -1};
			else if(subTerm->type.testFlag(SubTerm::Timepoint))
				isPoint = true;
		}
	}
	term.finalize();

	if(!rootTerm.isEmpty()) {
		if(isLoop) // (4)
			throw ErrorInfo{ErrorInfo::TermLevel, depth, LoopAsLimiterError, -1};

		// (5)
		auto fence = std::get<1>(rootTerm.splitLoop());
		if((static_cast<int>(fence.scope()) & static_cast<int>(term.scope())) != 0)
			throw ErrorInfo{ErrorInfo::TermLevel, depth, LimiterSmallerThanFenceError, -1};
		if(term.scope() <= fence.scope())
			throw ErrorInfo{ErrorInfo::TermLevel, depth, LimiterSmallerThanFenceError, -1};

		// (6)
		auto limiter = rootTerm.last().dynamicCast<LimiterTerm>();
		Q_ASSERT(limiter);
		limiter = limiter->clone(std::move(term));
		Q_ASSERT(limiter);
		rootTerm.last() = limiter;
		swap(term, rootTerm); //move the root to the actual term
	}
}

void EventExpressionParser::parseMultiTerm(QUuid id, const QString *expression, MultiTerm *terms)
{
	// first: find all subterms and prepare the multi term for them
	QRegularExpression splitRegex {
		QStringLiteral("\\s*(?:") + trList(ExpressionSeperator).join(QLatin1Char('|')) + QStringLiteral(")\\s*"),
		QRegularExpression::DontAutomaticallyOptimizeOption |
		QRegularExpression::CaseInsensitiveOption |
		QRegularExpression::UseUnicodePropertiesOption |
		QRegularExpression::DontCaptureOption
	};
	auto subExpressions = expression->splitRef(splitRegex, QString::SkipEmptyParts);
	terms->resize(subExpressions.size());

	// second: actually parse them. From here on the term is not edited anymore
	terms = nullptr;
	auto counter = 0;
	for(const auto &subExpr : subExpressions)
		parseTerm(id, subExpr, {}, counter++, {}, 0);

	completeTask(id);
}

template<typename TSubTerm>
void EventExpressionParser::parseSubTerm(EventExpressionParser::TermParams params)
{
	try {
		parseSubTermImpl<TSubTerm>(params.id,
								   params.expression,
								   std::move(params.term),
								   params.termIndex,
								   std::move(params.rootTerm),
								   params.depth);
	} catch(ErrorInfo &info) {
		info.subTermBegin = params.depth;
		reportError(params.id, info, true);
	}
}

template<typename TSubTerm>
void EventExpressionParser::parseSubTermImpl(QUuid id, const QStringRef &expression, Term term, int termIndex, Term rootTerm, int depth)
{
	static_assert(std::is_base_of<SubTerm, TSubTerm>::value, "TSubTerm must implement SubTerm");
	using ParseResult = std::pair<QSharedPointer<TSubTerm>, int>;
	ParseResult result = TSubTerm::parse(expression);
	if(result.first) {
		depth += result.second;
		term.append(result.first);
		validatePartialTerm(term, depth);
		if(result.second == expression.size()) {
			validateFullTerm(term, rootTerm, depth);
			emit termCompleted(id, termIndex, term);
		} else
			parseTerm(id, expression.mid(result.second), term, termIndex, rootTerm, depth);
	} else
		throw ErrorInfo{ErrorInfo::ParsingLevel, depth, ParserError, -1};

	completeTask(id);
}

template<>
void EventExpressionParser::parseSubTermImpl<LimiterTerm>(QUuid id, const QStringRef &expression, Term term, int termIndex, Term rootTerm, int depth)
{
	using ParseResult = std::pair<QSharedPointer<LimiterTerm>, int>;
	ParseResult result = LimiterTerm::parse(expression);
	if(result.first) {
		depth += result.second;
		if(!term.isEmpty()) {
			validateFullTerm(term, rootTerm, depth);
			term.append(result.first);
			validatePartialTerm(term, depth);
			parseTerm(id, expression.mid(result.second), {}, termIndex, term, depth);
		}
	} else
		throw ErrorInfo{ErrorInfo::ParsingLevel, depth, ParserError, -1};

	completeTask(id);
}

void EventExpressionParser::addTasks(QUuid id, int count)
{
	QReadLocker lock{&_taskLocker};
	_taskCounter[id].first += count;
}

void EventExpressionParser::reportError(QUuid id, EventExpressionParser::ErrorInfo info, bool autoComplete)
{
	const auto sig = info.calcSignificance();
	auto ok = false;
	QReadLocker lock{&_taskLocker};
	do { //try to set atomically. Needs 2 steps, first check if bigger, then set if unchanged
		const quint64 oldSig = _taskCounter[id].second;
		if(sig > oldSig) {
			ok = _taskCounter[id].second.testAndSetOrdered(oldSig, sig);
			if(ok)
				emit errorOccured(id, sig, info);
		} else
			ok = true; //old sig is "bigger", thus more important. This one can be skipped
	} while(!ok);

	if(autoComplete)
		completeTask(id, lock);
}

void EventExpressionParser::completeTask(QUuid id)
{
	QReadLocker lock{&_taskLocker};
	completeTask(id, lock);
}

void EventExpressionParser::completeTask(QUuid id, QReadLocker &)
{
	if(--_taskCounter[id].first == 0)
		emit operationCompleted(id);
}

QString EventExpressionParser::createErrorMessage(EventExpressionParser::ErrorType type, int depthEnd, const QStringRef &subTerm)
{
	switch (type) {
	case EventExpressionParser::NoError:
	case EventExpressionParser::UnknownError:
		return tr("Unknown Error");
	case EventExpressionParser::ParserError:
		return depthEnd == 0 ?
					tr("Unable to parse expression. Could not understand beginning of the expression.") :
					tr("Unable to parse expression. Was able to parse until position %L1, "
					   "but could not understand the part after this position.")
					.arg(depthEnd);

	case EventExpressionParser::DuplicateScopeError:
		return tr("Detected duplicate expression scope. Subterm \"%1\" conflicts with a previous subterm.")
				.arg(subTerm);
	case EventExpressionParser::DuplicateLoopError:
		return tr("Detected more then one loop expression. Subterm \"%1\" conflicts with a previous loop subterm.")
				.arg(subTerm);
	case EventExpressionParser::DuplicateSpanError:
		return tr("Detected more then one timespan expression. Subterm \"%1\" conflicts with a previous timespan subterm.<br/>"
				  "<b>Note:</b> You can specify multile timespans in one expression using something like \"in 2 hours and 20 minutes\".")
				.arg(subTerm);
	case EventExpressionParser::DuplicateFromLimiterError:
		return tr("Detected more then one \"from\" limiter expression. Subterm \"%1\" conflicts with a previous \"from\" limiter subterm.")
				.arg(subTerm);
	case EventExpressionParser::DuplicateUntilLimiterError:
		return tr("Detected more then one \"until\" limiter expression. Subterm \"%1\" conflicts with a previous \"until\" limiter subterm.")
				.arg(subTerm);
	case EventExpressionParser::UnexpectedLimiterError:
		return tr("Detected unexpected limiter expression. Subterm \"%1\" can only be used if a loop expression was previously used.")
				.arg(subTerm);

	case EventExpressionParser::UnexpectedAbsoluteSubTermError:
		return tr("Found an absolute subterm that is not the greatest scope. "
				  "Detected after checking the subterm until %L1 for inconsistencies.")
				.arg(depthEnd);
	case EventExpressionParser::SpanAfterTimepointError:
		return tr("Found a timespan expression of a logically smaller scope than a timepoint expression. This is not supported. "
				  "Detected after checking the subterm until %L1 for inconsistencies.")
				.arg(depthEnd);
	case EventExpressionParser::LoopAsLimiterError:
		return tr("Found a loop expression that is used as loop limiter. Limiters cannot be loops. "
				  "Detected after checking the subterm until %L1 for inconsistencies.")
				.arg(depthEnd);
	case EventExpressionParser::LimiterSmallerThanFenceError:
		return tr("Found a limiter expression of a logically smaller scope than the fencing part of a loop expression. "
				  "This is currently not supported due to a too high complexity. "
				  "Detected after checking the subterm until %L1 for inconsistencies.")
				.arg(depthEnd);

	case EventExpressionParser::TermIsLoopError:
		return tr("Tried to use a looped expression to get a single date. "
				  "You must use a normal, non-loop expression instead.");
	case EventExpressionParser::EvaluatesToPastError:
		return tr("The given expression, when applied to the current date, would evalute to the past. "
				  "Expressions must always evalute to a timepoint in the future.");
	case EventExpressionParser::UntilIsSmallerThenPastError:
		return tr("From and until limiter of the given expression are valid, "
				  "but from points to a timepoint further in the future than until. "
				  "The until expression must always be further in the past than the from expression.");
	case EventExpressionParser::InitialLoopInvalidError:
		return tr("The given looped expression, when applied to the current date to get the first occurence date, "
				  "did not return a valid date. This typically indicates that the application did not find any dates "
				  "from the current date on that match the expression. Make shure your expression evalutes to at least "
				  "one date in the future.");
	default:
		Q_UNREACHABLE();
		return {};
	}
}



quint64 EventExpressionParser::ErrorInfo::calcSignificance() const
{
	return (static_cast<quint64>(depth) << 32) | static_cast<quint64>(level);
}



SubTerm::SubTerm(Type type, Scope scope) :
	QObject{nullptr},
	type{type},
	scope{scope}
{}

SubTerm::SubTerm(QObject *parent) :
	QObject{parent}
{}

void SubTerm::fixup(QDateTime &datetime) const
{
	Q_UNUSED(datetime)
}

void SubTerm::fixupCleanup(QDateTime &datetime) const
{
	Q_UNUSED(datetime)
}

SubTerm::Type SubTerm::getType() const
{
	return type;
}

void SubTerm::setType(Type value)
{
	type = value;
}

SubTerm::Scope SubTerm::getScope() const
{
	return scope;
}

void SubTerm::setScope(Scope value)
{
	scope = value;
}


Term::Term(std::initializer_list<QSharedPointer<SubTerm>> args) :
	QList{args}
{}

Term::Term(const QList<QSharedPointer<SubTerm>> &list) :
	QList{list}
{
	finalize();
}

SubTerm::Scope Term::scope() const
{
	Q_ASSERT(isEmpty() || _scope != SubTerm::InvalidScope);
	return _scope;
}

bool Term::isLooped() const
{
	return _looped;
}

bool Term::isAbsolute() const
{
	return _absolute;
}

bool Term::hasTimeScope() const
{
	return _scope.testFlag(SubTerm::Hour) ||
			_scope.testFlag(SubTerm::Minute);
}

QDateTime Term::apply(const QDateTime &datetime, bool applyFenced) const
{
	auto appointment = datetime;
	for(const auto &term : *this) {
		if(term->type.testFlag(SubTerm::FlagLimiter)) // skip limiters when applying
			continue;
		term->apply(appointment, applyFenced);
		if(term->type.testFlag(SubTerm::Timepoint))
			applyFenced = true;
	}

	if(appointment <= datetime && !isEmpty()) {
		first()->fixup(appointment); //fix by applying an offset specific to the first subterm. might do nothing
		for(const auto &term : *this) {
			if(term->type.testFlag(SubTerm::FlagNeedsFixupCleanup))
				term->fixupCleanup(appointment);
		}
	}

	return appointment;
}

std::tuple<Term, Term, Term, Term> Term::splitLoop() const
{
	if(!isLooped())
		return {};

	auto splitIndex = -1;
	auto fromIndex = -1;
	auto toIndex = -1;
	for(auto i = 0; i < size(); ++i) {
		if(at(i)->type.testFlag(SubTerm::FlagLooped))
			splitIndex = i;
		else if(at(i)->type == SubTerm::FromSubterm)
			fromIndex = i;
		else if(at(i)->type == SubTerm::UntilSubTerm)
			toIndex = i;
	}

	// get the firs limiter index (both must be at the end of the sorted sub terms, because they are noscope
	int endIndex = -1;
	if(fromIndex != -1) {
		if(toIndex != -1)
			endIndex = std::min(fromIndex, toIndex);
		else
			endIndex = fromIndex;
	} else
		endIndex = toIndex;

	return std::make_tuple(
		splitIndex != -1 ? Term{mid(splitIndex, endIndex == -1 ? -1 : endIndex - splitIndex)} : Term{mid(0, endIndex)},
		splitIndex != -1 ? Term{mid(0, splitIndex)} : Term{},
		fromIndex != -1 ? at(fromIndex).staticCast<LimiterTerm>()->limitTerm() : Term{},
		toIndex != -1 ? at(toIndex).staticCast<LimiterTerm>()->limitTerm() : Term{}
						);
}

QString Term::describe() const
{
	if(isLooped()) {
		Term loop, fence, from, until;
		std::tie(loop, fence, from, until) = splitLoop();
		QString res;
		if(!fence.isEmpty())
			res += EventExpressionParser::tr("within {%1} ").arg(fence.describeImpl());
		res += EventExpressionParser::tr("every {%1}").arg(loop.describeImpl());
		if(!from.isEmpty())
			res += EventExpressionParser::tr(" from {%1}").arg(from.describeImpl());
		if(!until.isEmpty())
			res += EventExpressionParser::tr(" until {%1}").arg(until.describeImpl());
		return res;
	} else
		return describeImpl();
}

void Term::finalize()
{
	_scope = SubTerm::InvalidScope;
	_looped = false;
	_absolute = false;
	for(const auto &subTerm : qAsConst(*this)) {
		_scope |= subTerm->scope;
		_looped = _looped || subTerm->type.testFlag(SubTerm::FlagLooped);
		_absolute = _absolute || subTerm->type.testFlag(SubTerm::FlagAbsolute);
	}
}

QString Term::describeImpl() const
{
	QStringList subDesc;
	subDesc.reserve(size());
	for(const auto &term : *this)
		subDesc.append(term->describe());
	return subDesc.join(QStringLiteral("; "));
}

QString Expressions::describeMultiTerm(const MultiTerm &term, bool asHtml)
{
	QStringList descs;
	descs.reserve(term.size());
	for(const auto &termSel : term) {
		Q_ASSERT(termSel.size() == 1);
		if(asHtml)
			descs.append(termSel.first().describe().toHtmlEscaped());
		else
			descs.append(termSel.first().describe());
	}
	return descs.join(asHtml ? QStringLiteral("<br/>") : QStringLiteral("\n"));
}



EventExpressionParserException::EventExpressionParserException(EventExpressionParser::ErrorType type, int depthEnd, const QStringRef &subTerm) :
	_type{type},
	_message{EventExpressionParser::createErrorMessage(type, depthEnd, subTerm)},
	_what{"Error " + QByteArray::number(_type) + ": " + _message.toUtf8()}
{}

EventExpressionParserException::EventExpressionParserException(QString message) :
	_type{EventExpressionParser::UnknownError},
	_message{std::move(message)},
	_what{"Error " + QByteArray::number(_type) + ": " + _message.toUtf8()}
{}

QString EventExpressionParserException::message() const
{
	return _message;
}

EventExpressionParser::ErrorType EventExpressionParserException::type() const
{
	return _type;
}

EventExpressionParserException::EventExpressionParserException(const EventExpressionParserException * const other) :
	_type{other->_type},
	_message{other->_message},
	_what{other->_what}
{}

QString EventExpressionParserException::qWhat() const
{
	return QString::fromUtf8(_what);
}

const char *EventExpressionParserException::what() const noexcept
{
	return _what.constData();
}

void EventExpressionParserException::raise() const
{
	throw *this;
}

QException *EventExpressionParserException::clone() const
{
	return new EventExpressionParserException{this};
}
