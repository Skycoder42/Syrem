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
			loop.append(QSharedPointer<TimeTerm>::create(_settings->scheduler.defaultTime, true));
			loop.finalize();
		}

		// get the from date
		QDateTime fromDate;
		if(!from.isEmpty()) {
			if(!from.hasTimeScope()) {
				from.append(QSharedPointer<TimeTerm>::create(QTime{0, 0}, true));
				from.finalize();
			}
			fromDate = evaluteTerm(from, reference); //TODO errors here and below
		}
		if(!fromDate.isValid())
			fromDate = reference;

		// get the until date
		QDateTime untilDate;
		if(!until.isEmpty()) {
			if(!until.hasTimeScope()) {
				until.append(QSharedPointer<TimeTerm>::create(QTime{23, 59}, true));
				until.finalize();
			}
			untilDate = evaluteTerm(until, reference);
		}

		if(fromDate.isValid() && untilDate.isValid() && untilDate <= fromDate)
			return {}; //TODO error

		// create schedule and getNext once for the initial date
		auto res = QSharedPointer<RepeatedSchedule>::create(loop, fence, fromDate, untilDate);
		if(res->nextSchedule().isValid())
			return res;
		else
			return {};
	} else {
		const auto then = evaluteTerm(term, reference);
		if(then.isValid())
			return QSharedPointer<SingularSchedule>::create(then); // create a "pre-evaluated" one time schedule
		else
			return {};
	}
}

QDateTime EventExpressionParser::evaluteTerm(const Term &term, const QDateTime &reference)
{
	if(term.isLooped())
		return {};

	auto then = term.apply(reference);
	if(!term.hasTimeScope()) {
		QTime time = _settings->scheduler.defaultTime;
		if(time.isValid() && time != QTime{0,0})
			then.setTime(time);
	}
	if(reference < then)
		return then;
	else
		return {};
}

MultiTerm EventExpressionParser::parseExpressionImpl(const QString &expression, bool allowMulti)
{
	// prepare eventloop with result signal handlers
	const auto id = QUuid::createUuid();
	MultiTerm terms;
	QEventLoop loop;
	connect(this, &EventExpressionParser::termCompleted, &loop, [&](QUuid termId, int termIndex, const Term &term){
		if(termId == id)
			terms[termIndex].append(term);
	}, Qt::QueuedConnection);
	connect(this, &EventExpressionParser::operationCompleted, &loop, [&](QUuid doneId){
		if(doneId == id)
			loop.quit();
	}, Qt::QueuedConnection);

	// start operations
	{
		QWriteLocker lock{&_taskLocker};
		_taskCounter.insert(id, 1);
	}
	if(allowMulti)
		QtConcurrent::run(this, &EventExpressionParser::parseMultiTerm, id, &expression, &terms);
	else {
		terms.append(TermSelection{});
		//parseTerm must be directly called. The manual call to complete is only needed here, as only the async methods do that
		parseTerm(id, &expression, {}, 0, {});
		completeTask(id);
	}

	auto res = loop.exec();
	{
		QWriteLocker lock{&_taskLocker};
		Q_ASSERT(_taskCounter.value(id) == 0);
		_taskCounter.remove(id);
	}
	if(res == EXIT_SUCCESS)
		return terms;
	else
		return {};
}

void EventExpressionParser::parseTerm(QUuid id, const QStringRef &expression, const Term &term, int termIndex, const Term &rootTerm)
{
	// start parser-tasks for all the possible subterms
	addTasks(id, 10);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<TimeTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<DateTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<InvertedTimeTerm>, id, expression, term, termIndex, rootTerm);

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<MonthDayTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<WeekDayTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<MonthTerm>, id, expression, term, termIndex, rootTerm);

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<YearTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<SequenceTerm>, id, expression, term, termIndex, rootTerm);
	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<KeywordTerm>, id, expression, term, termIndex, rootTerm);

	QtConcurrent::run(this, &EventExpressionParser::parseSubTerm<LimiterTerm>, id, expression, term, termIndex, rootTerm);
}

bool EventExpressionParser::validatePartialTerm(const Term &term)
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
			return false;
		allScope |= subTerm->scope;

		// (2)
		if(subTerm->type.testFlag(SubTerm::FlagLooped)) {
			if(hasLoop)
				return false;
			else
				hasLoop = true;
		}
		// (3)
		if(subTerm->type.testFlag(SubTerm::Timespan)) {
			if(hasSpan)
				return false;
			else
				hasSpan = true;
		}
		// (4)
		if(subTerm->type == SubTerm::FromSubterm) {
			if(hasFromLimiter)
				return false;
			else
				hasFromLimiter = true;
		}
		if(subTerm->type == SubTerm::UntilSubTerm) {
			if(hasUntilLimiter)
				return false;
			else
				hasUntilLimiter = true;
		}
	}

	if((hasFromLimiter || hasUntilLimiter) && !hasLoop) // (5)
		return false;

	return true;
}

bool EventExpressionParser::validateFullTerm(Term &term, Term &rootTerm)
{
	/* Checks to perform on the full term:
	 *	1. Sort by scope
	 *	2. Only the first element can be absolute
	 *	3. Loops must not be followed by spans
	 *	4. Timepoints must not be followed by spans, except for loops (as the point part serves as "fence")
	 *
	 * If rootTerm is "valid", aka term is a limiter:
	 *	5. Limiters must not be loops
	 *	6. Verify limiters are "bigger" in scope then the loop fence, if present
	 *	7. Merge the term into root term and swap them
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
			return false;

		// (3)
		if(isLoop && subTerm->type.testFlag(SubTerm::Timespan))
			return false;
		else if(subTerm->type.testFlag(SubTerm::FlagLooped))
			isLoop = true;

		// (4)
		if(!isLoop) {
			if(isPoint && subTerm->type.testFlag(SubTerm::Timespan))
				return false;
			else if(subTerm->type.testFlag(SubTerm::Timepoint))
				isPoint = true;
		}
	}
	term.finalize();

	if(!rootTerm.isEmpty()) {
		if(isLoop) // (4)
			return false;

		// (6)
		auto fence = std::get<1>(rootTerm.splitLoop());
		if((static_cast<int>(fence.scope()) & static_cast<int>(term.scope())) != 0)
			return false;
		if(term.scope() <= fence.scope())
			return false;

		// (7)
		auto limiter = rootTerm.last().dynamicCast<LimiterTerm>();
		Q_ASSERT(limiter);
		limiter->_limitTerm = Term {};
		swap(limiter->_limitTerm, term); //move term into the limiter
		swap(term, rootTerm); //move the root to the actual term
	}

	return true;
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
		parseTerm(id, subExpr, {}, counter++, {});

	completeTask(id);
}

template<typename TSubTerm>
void EventExpressionParser::parseSubTerm(QUuid id, const QStringRef &expression, Term term, int termIndex, Term rootTerm)
{
	static_assert(std::is_base_of<SubTerm, TSubTerm>::value, "TSubTerm must implement SubTerm");
	using ParseResult = std::pair<QSharedPointer<TSubTerm>, int>;
	ParseResult result = TSubTerm::parse(expression);
	if(result.first) {
		term.append(result.first);
		if(!validatePartialTerm(term))
			; //TODO report errors
		else if(result.second == expression.size()) {
			if(validateFullTerm(term, rootTerm))
				emit termCompleted(id, termIndex, term);
			else
				; //TODO report errors
		} else
			parseTerm(id, expression.mid(result.second), term, termIndex, rootTerm);
	}
	completeTask(id);
}

template<>
void EventExpressionParser::parseSubTerm<LimiterTerm>(QUuid id, const QStringRef &expression, Term term, int termIndex, Term rootTerm)
{
	using ParseResult = std::pair<QSharedPointer<LimiterTerm>, int>;
	ParseResult result = LimiterTerm::parse(expression);
	if(result.first) {
		if(!term.isEmpty()) {
			if(validateFullTerm(term, rootTerm)) {
				term.append(result.first);
				if(validatePartialTerm(term))
					parseTerm(id, expression.mid(result.second), {}, termIndex, term);
				else
					; //TODO report errors
			} else
				; //TODO report errors
		}
	}
	completeTask(id);
}

void EventExpressionParser::addTasks(QUuid id, int count)
{
	QReadLocker lock{&_taskLocker};
	_taskCounter[id] += count;
}

void EventExpressionParser::completeTask(QUuid id)
{
	QReadLocker lock{&_taskLocker};
	if(--_taskCounter[id] == 0)
		emit operationCompleted(id);
}



SubTerm::SubTerm(Type t, Scope s, bool c) :
	QObject{nullptr},
	type{t},
	scope{s},
	certain{c}
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
