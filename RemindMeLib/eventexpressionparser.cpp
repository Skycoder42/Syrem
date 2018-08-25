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
			fromDate = evaluteTerm(from, reference); //TODO test errors here and below
		}
		if(!fromDate.isValid())
			fromDate = reference;

		// get the until date
		QDateTime untilDate;
		if(!until.isEmpty()) {
			if(!until.hasTimeScope()) {
				until.append(QSharedPointer<TimeTerm>::create(QTime{23, 59}));
				until.finalize();
			}
			untilDate = evaluteTerm(until, reference);
		}

		if(fromDate.isValid() && untilDate.isValid() && untilDate <= fromDate)
			throw EventExpressionParserException{UntilIsSmallerThenPastError, {}}; //TODO msg

		// create schedule and getNext once for the initial date
		auto res = QSharedPointer<RepeatedSchedule>::create(loop, fence, fromDate, untilDate);
		if(res->nextSchedule().isValid())
			return res;
		else
			throw EventExpressionParserException{InitialLoopInvalidError, {}}; //TODO msg
	} else
		return QSharedPointer<SingularSchedule>::create(evaluteTerm(term, reference)); // create a "pre-evaluated" one time schedule
}

QDateTime EventExpressionParser::evaluteTerm(const Term &term, const QDateTime &reference)
{
	if(term.isLooped())
		throw EventExpressionParserException{TermIsLoopError, {}}; //TODO msg

	auto then = term.apply(reference);
	if(!term.hasTimeScope()) {
		QTime time = _settings->scheduler.defaultTime;
		if(time.isValid() && time != QTime{0,0})
			then.setTime(time);
	}
	if(reference < then)
		return then;
	else
		throw EventExpressionParserException{EvaluatesToPastError, {}}; //TODO msg
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
	connect(this, &EventExpressionParser::errorOccured, &loop, [&](QUuid termId, quint64 significance, const ErrorInfo &error){
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
		for(const auto &term : terms) { // throw error for the first subterm that failed
			if(term.isEmpty())
				throw EventExpressionParserException{lastError.type, {}};//TODO generate message
		}
		return terms;
	} else
		throw EventExpressionParserException{UnknownError, tr("Unknown error")};
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
		limiter->_limitTerm = Term {};
		swap(limiter->_limitTerm, term); //move term into the limiter
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

void EventExpressionParser::reportError(QUuid id, const ErrorInfo &info, bool autoComplete)
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



quint64 EventExpressionParser::ErrorInfo::calcSignificance() const
{
	return (static_cast<quint64>(depth) << 32) | static_cast<quint64>(level);
}



SubTerm::SubTerm(Type t, Scope s) :
	QObject{nullptr},
	type{t},
	scope{s}
{}

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

QDateTime Term::apply(QDateTime datetime, bool applyRelative) const
{
	for(const auto &term : *this) {
		if(term->type.testFlag(SubTerm::FlagLimiter)) // skip limiters when applying
			continue;
		term->apply(datetime, applyRelative);
		applyRelative = false;
	}
	return datetime;
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



EventExpressionParserException::EventExpressionParserException(EventExpressionParser::ErrorType type, QString message) :
	_type{type},
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
