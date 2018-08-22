#include "eventexpressionparser.h"
#include "schedule.h"
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

QSharedPointer<Schedule> EventExpressionParser::parseSchedule(const Term &term)
{

}

QDateTime EventExpressionParser::parseSnoozeTime(const Term &term)
{
	const auto now = QDateTime::currentDateTime();
	auto then = term.apply(now);
	if(!term.scope().testFlag(Hour) &&
	   !term.scope().testFlag(Minute)) {
		QTime time = _settings->scheduler.defaultTime;
		if(time.isValid() && time != QTime{0,0})
			then.setTime(time);
	}
	if(now >= then)
		return {};
	else
		return then;
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
	 *	3. Verify there is only a single limiter of each type
	 *	4. Only loops can have limiters
	 */
	Scope allScope = InvalidScope;
	auto hasLoop = false;
	auto hasFromLimiter = false;
	auto hasUntilLimiter = false;
	for(const auto &subTerm : term) {
		if((static_cast<int>(allScope) & static_cast<int>(subTerm->scope)) != 0) // (1)
			return false;
		if(subTerm->type.testFlag(FlagLooped)) {
			if(hasLoop) // (2)
				return false;
			else
				hasLoop = true;
		}
		allScope |= subTerm->scope;

		// (3)
		if(subTerm->type == FromSubterm) {
			if(hasFromLimiter)
				return false;
			else
				hasFromLimiter = true;
		}
		if(subTerm->type == UntilSubTerm) {
			if(hasUntilLimiter)
				return false;
			else
				hasUntilLimiter = true;
		}
	}

	if((hasFromLimiter || hasUntilLimiter) && !hasLoop) // (4)
		return false;

	return true;
}

bool EventExpressionParser::validateFullTerm(Term &term, Term &rootTerm)
{
	/* Checks to perform on the full term:
	 *	1. Sort by scope...
	 *	2. Only the first element can be absolute
	 *	3. Timepoints must not be followed by spans TODO except for loops
	 *
	 *	4. If rootTerm is "valid", then merge the term into root term and swap them
	 */
	std::sort(term.begin(), term.end(), [](const QSharedPointer<SubTerm> &lhs, const QSharedPointer<SubTerm> &rhs){
		return lhs->scope < rhs->scope;
	});

	auto isFirst = true;
	auto isPoint = false;
	for(const auto &subTerm : qAsConst(term)) {
		if(isFirst)
			isFirst = false;
		else if(subTerm->type.testFlag(FlagAbsolute)) // (2)
			return false;

		if(isPoint && subTerm->type.testFlag(Timespan)) // (3)
			return false;
		else if(subTerm->type.testFlag(Timepoint))
			isPoint = true;
	}
	term.finalize();

	if(!rootTerm.isEmpty()) { // (4)
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





SubTerm::~SubTerm() = default;



TimeTerm::TimeTerm(QTime time, bool certain) :
	SubTerm{Timepoint, Hour | Minute, certain},
	_time{time}
{}

std::pair<QSharedPointer<TimeTerm>, int> TimeTerm::parse(const QStringRef &expression)
{
	const QLocale locale;
	const auto prefix = QStringLiteral("(%1)?").arg(trList(TimePrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(TimeSuffix).join(QLatin1Char('|')));

	for(const auto &pattern : trList(TimePattern, false)) {
		QRegularExpression regex {
			QLatin1Char('^') + prefix + QLatin1Char('(') + toRegex(pattern) + QLatin1Char(')') + suffix + QStringLiteral("\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = regex.match(expression);
		if(match.hasMatch()) {
			auto time = locale.toTime(match.captured(2), pattern);
			if(time.isValid()) {
				return {
					QSharedPointer<TimeTerm>::create(time, match.capturedLength(1) > 0 || match.capturedLength(3) > 0),
					match.capturedLength(0)
				};
			}
		}
	}

	return {};
}

void TimeTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	Q_UNUSED(applyRelative)
	datetime.setTime(_time);
}

QString TimeTerm::toRegex(QString pattern)
{
	const QLocale locale;
	return dateTimeFormatToRegex(std::move(pattern), [&locale](QString &text) {
		text.replace(QStringLiteral("hh"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("h"), QStringLiteral("\\d{1,2}"))
				.replace(QStringLiteral("mm"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("m"), QStringLiteral("\\d{1,2}"))
				.replace(QStringLiteral("ss"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("s"), QStringLiteral("\\d{1,2}"))
				.replace(QStringLiteral("zzz"), QStringLiteral("\\d{3}"))
				.replace(QStringLiteral("z"), QStringLiteral("\\d{1,3}"))
				.replace(QStringLiteral("ap"),
						 QStringLiteral("(?:%1|%2)")
						 .arg(QRegularExpression::escape(locale.amText()),
							  QRegularExpression::escape(locale.pmText())),
						 Qt::CaseInsensitive);
	});
}



DateTerm::DateTerm(QDate date, bool hasYear, bool certain) :
	SubTerm{hasYear ? AbsoluteTimepoint : Timepoint,
			(hasYear ? Year : InvalidScope) | Month | MonthDay,
			certain},
	_date{date}
{}

std::pair<QSharedPointer<DateTerm>, int> DateTerm::parse(const QStringRef &expression)
{
	const QLocale locale;
	const auto prefix = QStringLiteral("(%1)?").arg(trList(DatePrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(DateSuffix).join(QLatin1Char('|')));

	for(const auto &pattern : trList(DatePattern, false)) {
		bool hasYear = false;
		QRegularExpression regex {
			QLatin1Char('^') + prefix + QLatin1Char('(') + toRegex(pattern, hasYear) + QLatin1Char(')') + suffix + QStringLiteral("\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = regex.match(expression);
		if(match.hasMatch()) {
			auto date = locale.toDate(match.captured(2), pattern);
			if(date.isValid()) {
				return {
					QSharedPointer<DateTerm>::create(date,
													 hasYear,
													 match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
					match.capturedLength(0)
				};
			}
		}
	}

	return {};
}

void DateTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	if(scope.testFlag(Year)) //set the whole date
		datetime.setDate(_date);
	else { // set only day and month, keep year
		QDate newDate {
			datetime.date().year(),
			_date.month(),
			_date.day()
		};
		if(applyRelative && datetime.date() >= newDate)
			newDate = newDate.addYears(1);
		datetime.setDate(newDate);
	}
}

QString DateTerm::toRegex(QString pattern, bool &hasYear)
{
	hasYear = false;
	return dateTimeFormatToRegex(std::move(pattern), [&hasYear](QString &text) {
		text.replace(QStringLiteral("dd"), QStringLiteral("\\d{2}"))
				.replace(QRegularExpression{QStringLiteral(R"__((?<!\\)((?:\\{2})*)d)__")}, QStringLiteral("\\1\\d{1,2}")) //special regex to not include previous "\d" replacements, but allow "\\d"
				.replace(QStringLiteral("MM"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("M"), QStringLiteral("\\d{1,2}"));
		auto oldLen = text.size();
		text.replace(QStringLiteral("yyyy"), QStringLiteral("-?\\d{4}"))
				.replace(QStringLiteral("yy"), QStringLiteral("\\d{2}"));
		hasYear = hasYear || (text.size() != oldLen);
	});
}



InvertedTimeTerm::InvertedTimeTerm(QTime time) :
	SubTerm{Timepoint, Hour | Minute, true},
	_time{time}
{}

std::pair<QSharedPointer<InvertedTimeTerm>, int> InvertedTimeTerm::parse(const QStringRef &expression)
{
	const QLocale locale;
	// prepare suffix/prefix
	const auto prefix = QStringLiteral("(?:%1)?").arg(trList(TimePrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(?:%1)?").arg(trList(TimeSuffix).join(QLatin1Char('|')));

	// prepare primary expression patterns
	QHash<QString, int> keywordMap;
	QString keywordRegexStr;
	for(const auto &mapping : trList(InvTimeKeyword, false)) {
		const auto split = mapping.split(QLatin1Char(':'));
		Q_ASSERT_X(split.size() == 2, Q_FUNC_INFO, "Invalid InvTimeKeyword translation. Must be keyword and value, seperated by a ':'");
		keywordMap.insert(split[0], split[1].toInt());
		keywordRegexStr.append(QLatin1Char('|') + QRegularExpression::escape(split[0]));
	}
	// prepare hour/minute patterns
	QVector<std::pair<QString, QString>> hourPatterns;
	{
		const auto pList = trList(InvTimeHourPattern, false);
		hourPatterns.reserve(pList.size());
		for(auto &pattern : pList)
			hourPatterns.append({pattern, hourToRegex(pattern)});
	}
	QVector<std::pair<QString, QString>> minPatterns;
	{
		const auto pList = trList(InvTimeMinutePattern, false);
		minPatterns.reserve(pList.size());
		for(auto &pattern : pList)
			minPatterns.append({pattern, minToRegex(pattern)});
	}

	for(const auto &exprPattern : trList(InvTimeExprPattern, false)) {
		const auto split = exprPattern.split(QLatin1Char(':'));
		Q_ASSERT_X(split.size() == 2 && (split[1] == QLatin1Char('+') || split[1] == QLatin1Char('-')),
				Q_FUNC_INFO,
				"Invalid InvTimePattern translation. Must be an expression and sign (+/-), seperated by a ':'");

		for(const auto &hourPattern : hourPatterns) {
			for(const auto &minPattern : minPatterns) {
				QRegularExpression regex {
					QLatin1Char('^') + prefix +
					split[0].arg(QStringLiteral(R"__((?<hours>%1))__").arg(hourPattern.second),
								 QStringLiteral(R"__((?<minutes>%1%2))__").arg(minPattern.second, keywordRegexStr)) +
					suffix + QStringLiteral("\\s*"),
					QRegularExpression::DontAutomaticallyOptimizeOption |
					QRegularExpression::CaseInsensitiveOption |
					QRegularExpression::UseUnicodePropertiesOption
				};
				auto match = regex.match(expression);
				if(match.hasMatch()) {
					// extract minutes and hours from the expression
					auto hours = locale.toTime(match.captured(QStringLiteral("hours")), hourPattern.first).hour();
					auto minutesStr = match.captured(QStringLiteral("minutes"));
					auto minutes = keywordMap.contains(minutesStr) ?
									   keywordMap.value(minutesStr) :
									   locale.toTime(minutesStr, minPattern.first).minute();
					//negative minutes (i.e. 10 to 4 -> 3:50)
					if(split[1] == QLatin1Char('-')) {
						hours = (hours == 0 ? 23 : hours - 1);
						minutes = 60 - minutes;
					}
					QTime time{hours, minutes};
					if(time.isValid()) {
						return {
							QSharedPointer<InvertedTimeTerm>::create(time),
							match.capturedLength(0)
						};
					}
				}
			}
		}
	}

	return {};
}

void InvertedTimeTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	Q_UNUSED(applyRelative)
	datetime.setTime(_time);
}

QString InvertedTimeTerm::hourToRegex(QString pattern)
{
	const QLocale locale;
	return dateTimeFormatToRegex(std::move(pattern), [&locale](QString &text) {
		text.replace(QStringLiteral("hh"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("h"), QStringLiteral("\\d{1,2}"))
				.replace(QStringLiteral("ap"),
						 QStringLiteral("(?:%1|%2)")
						 .arg(QRegularExpression::escape(locale.amText()),
							  QRegularExpression::escape(locale.pmText())),
						 Qt::CaseInsensitive);
	});
}

QString InvertedTimeTerm::minToRegex(QString pattern)
{
	return dateTimeFormatToRegex(std::move(pattern), [](QString &text) {
		text.replace(QStringLiteral("mm"), QStringLiteral("\\d{2}"))
				.replace(QStringLiteral("m"), QStringLiteral("\\d{1,2}"));
	});
}



MonthDayTerm::MonthDayTerm(int day, bool looped, bool certain) :
	SubTerm{looped ? LoopedTimePoint : Timepoint, MonthDay, certain},
	_day{day}
{}

std::pair<QSharedPointer<MonthDayTerm>, int> MonthDayTerm::parse(const QStringRef &expression)
{
	// get and prepare standard *fixes and indicators
	const auto prefix = QStringLiteral("(%1)?").arg(trList(MonthDayPrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(MonthDaySuffix).join(QLatin1Char('|')));
	auto indicators = trList(MonthDayIndicator, false);
	for(auto &indicator : indicators) {
		const auto split = indicator.split(QLatin1Char('_'));
		Q_ASSERT_X(split.size() == 2, Q_FUNC_INFO, "Invalid MonthDayIndicator translation. Must be some indicator text with a '_' as date placeholder");
		indicator = QRegularExpression::escape(split[0]) + QStringLiteral("(\\d{1,2})") + QRegularExpression::escape(split[1]);
	}

	// prepare list of combos to try. can be {loop, suffix}, {prefix, loop} or {prefix, suffix}, but the first two only if a loop*fix is defined
	QVector<std::tuple<QString, QString, bool>> exprCombos;
	exprCombos.reserve(3);
	{
		const auto loopPrefix = trList(MonthDayLoopPrefix);
		if(!loopPrefix.isEmpty())
			exprCombos.append(std::make_tuple(QStringLiteral("(%1)").arg(loopPrefix.join(QLatin1Char('|'))), suffix, true));
	}
	{
		const auto loopSuffix = trList(MonthDayLoopSuffix);
		if(!loopSuffix.isEmpty())
			exprCombos.append(std::make_tuple(prefix, QStringLiteral("(%1)").arg(loopSuffix.join(QLatin1Char('|'))), true));
	}
	exprCombos.append(std::make_tuple(prefix, suffix, false));

	// then loop through all the combinations and try to find one
	for(const auto &loopCombo : exprCombos) {
		for(const auto &indicator : indicators) {
			QRegularExpression regex {
				QLatin1Char('^') + std::get<0>(loopCombo) + indicator + std::get<1>(loopCombo) + QStringLiteral("\\s*"),
				QRegularExpression::DontAutomaticallyOptimizeOption |
				QRegularExpression::CaseInsensitiveOption |
				QRegularExpression::UseUnicodePropertiesOption
			};
			auto match = regex.match(expression);
			if(match.hasMatch()) {
				bool ok = false;
				auto day = match.captured(2).toInt(&ok);
				if(ok && day >= 1 && day <= 31) {
					auto isLoop = std::get<2>(loopCombo);
					return {
						QSharedPointer<MonthDayTerm>::create(day, isLoop,
															 isLoop || match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
						match.capturedLength(0)
					};
				}
			}
		}
	}

	return {};
}

void MonthDayTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	auto date = datetime.date();
	if(applyRelative && date.day() >= std::min(_day, date.daysInMonth())) // compare with shortend date
		date = date.addMonths(1);
	datetime.setDate({
		date.year(),
		date.month(),
		std::min(_day, date.daysInMonth()) // shorten day to target month
	});
}



WeekDayTerm::WeekDayTerm(int weekDay, bool looped, bool certain) :
	SubTerm{looped ? LoopedTimePoint : Timepoint, WeekDay, certain},
	_weekDay{weekDay}
{}

std::pair<QSharedPointer<WeekDayTerm>, int> WeekDayTerm::parse(const QStringRef &expression)
{
	const QLocale locale;
	// get and prepare standard *fixes and indicators
	const auto prefix = QStringLiteral("(%1)?").arg(trList(WeekDayPrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(WeekDaySuffix).join(QLatin1Char('|')));
	QString shortDays;
	QString longDays;
	{
		QStringList sList;
		QStringList lList;
		sList.reserve(14);
		lList.reserve(14);
		for(auto i = 1; i <= 7; i++) {
			sList.append(QRegularExpression::escape(locale.dayName(i, QLocale::ShortFormat)));
			sList.append(QRegularExpression::escape(locale.standaloneDayName(i, QLocale::ShortFormat)));
			lList.append(QRegularExpression::escape(locale.dayName(i, QLocale::LongFormat)));
			lList.append(QRegularExpression::escape(locale.standaloneDayName(i, QLocale::LongFormat)));
		}
		sList.removeDuplicates();
		lList.removeDuplicates();
		shortDays = sList.join(QLatin1Char('|'));
		longDays = lList.join(QLatin1Char('|'));
	}

	// prepare list of combos to try. can be {loop, suffix}, {prefix, loop} or {prefix, suffix}, but the first two only if a loop*fix is defined
	QVector<std::tuple<QString, QString, bool>> exprCombos;
	exprCombos.reserve(3);
	{
		const auto loopPrefix = trList(WeekDayLoopPrefix);
		if(!loopPrefix.isEmpty())
			exprCombos.append(std::make_tuple(QStringLiteral("(%1)").arg(loopPrefix.join(QLatin1Char('|'))), suffix, true));
	}
	{
		const auto loopSuffix = trList(WeekDayLoopSuffix);
		if(!loopSuffix.isEmpty())
			exprCombos.append(std::make_tuple(prefix, QStringLiteral("(%1)").arg(loopSuffix.join(QLatin1Char('|'))), true));
	}
	exprCombos.append(std::make_tuple(prefix, suffix, false));

	// then loop through all the combinations and try to find one
	for(const auto &loopCombo : exprCombos) {
		for(const auto &dayType : {
				std::make_pair(longDays, QStringLiteral("dddd")),
				std::make_pair(shortDays, QStringLiteral("ddd"))
			}) {
			QRegularExpression regex {
				QLatin1Char('^') + std::get<0>(loopCombo) +
				QLatin1Char('(') + dayType.first + QLatin1Char(')') +
				std::get<1>(loopCombo) + QStringLiteral("\\s*"),
				QRegularExpression::DontAutomaticallyOptimizeOption |
				QRegularExpression::CaseInsensitiveOption |
				QRegularExpression::UseUnicodePropertiesOption
			};
			auto match = regex.match(expression);
			if(match.hasMatch()) {
				auto dayName = match.captured(2);
				auto dDate = locale.toDate(dayName, dayType.second);
				if(dDate.isValid()) {
					auto isLoop = std::get<2>(loopCombo);
					return {
						QSharedPointer<WeekDayTerm>::create(dDate.dayOfWeek(), isLoop,
															isLoop || match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
						match.capturedLength(0)
					};
				}
			}
		}
	}

	return {};
}

void WeekDayTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	auto date = datetime.date();
	// get the number of days to add to reach the target date
	auto dayDiff = _weekDay - date.dayOfWeek();
	if(applyRelative && dayDiff <= 0) //make days positive into the next week
		dayDiff = 7 + dayDiff; // + because diff is already negative
	date = date.addDays(dayDiff);
	Q_ASSERT(date.dayOfWeek() == _weekDay);
	datetime.setDate(date);
}



MonthTerm::MonthTerm(int month, bool looped, bool certain) :
	SubTerm{looped ? LoopedTimePoint : Timepoint, Month, certain},
	_month{month}
{}

std::pair<QSharedPointer<MonthTerm>, int> MonthTerm::parse(const QStringRef &expression)
{
	const QLocale locale;
	// get and prepare standard *fixes and indicators
	const auto prefix = QStringLiteral("(%1)?").arg(trList(MonthPrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(MonthSuffix).join(QLatin1Char('|')));
	QString shortMonths;
	QString longMonths;
	{
		QStringList sList;
		QStringList lList;
		sList.reserve(24);
		lList.reserve(24);
		for(auto i = 1; i <= 12; i++) {
			sList.append(QRegularExpression::escape(locale.monthName(i, QLocale::ShortFormat)));
			sList.append(QRegularExpression::escape(locale.standaloneMonthName(i, QLocale::ShortFormat)));
			lList.append(QRegularExpression::escape(locale.monthName(i, QLocale::LongFormat)));
			lList.append(QRegularExpression::escape(locale.standaloneMonthName(i, QLocale::LongFormat)));
		}
		sList.removeDuplicates();
		lList.removeDuplicates();
		shortMonths = sList.join(QLatin1Char('|'));
		longMonths = lList.join(QLatin1Char('|'));
	}

	// prepare list of combos to try. can be {loop, suffix}, {prefix, loop} or {prefix, suffix}, but the first two only if a loop*fix is defined
	QVector<std::tuple<QString, QString, bool>> exprCombos;
	exprCombos.reserve(3);
	{
		const auto loopPrefix = trList(MonthLoopPrefix);
		if(!loopPrefix.isEmpty())
			exprCombos.append(std::make_tuple(QStringLiteral("(%1)").arg(loopPrefix.join(QLatin1Char('|'))), suffix, true));
	}
	{
		const auto loopSuffix = trList(MonthLoopSuffix);
		if(!loopSuffix.isEmpty())
			exprCombos.append(std::make_tuple(prefix, QStringLiteral("(%1)").arg(loopSuffix.join(QLatin1Char('|'))), true));
	}
	exprCombos.append(std::make_tuple(prefix, suffix, false));

	// then loop through all the combinations and try to find one
	for(const auto &loopCombo : exprCombos) {
		for(const auto &monthType : {
				std::make_pair(longMonths, QStringLiteral("MMMM")),
				std::make_pair(shortMonths, QStringLiteral("MMM"))
			}) {
			QRegularExpression regex {
				QLatin1Char('^') + std::get<0>(loopCombo) +
				QLatin1Char('(') + monthType.first + QLatin1Char(')') +
				std::get<1>(loopCombo) + QStringLiteral("\\s*"),
				QRegularExpression::DontAutomaticallyOptimizeOption |
				QRegularExpression::CaseInsensitiveOption |
				QRegularExpression::UseUnicodePropertiesOption
			};
			auto match = regex.match(expression);
			if(match.hasMatch()) {
				auto monthName = match.captured(2);
				auto mDate = locale.toDate(monthName, monthType.second);
				if(mDate.isValid()) {
					auto isLoop = std::get<2>(loopCombo);
					return {
						QSharedPointer<MonthTerm>::create(mDate.month(), isLoop,
														  isLoop || match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
						match.capturedLength(0)
					};
				}
			}
		}
	}

	return {};
}

void MonthTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	auto date = datetime.date();
	if(applyRelative && date.month() >= _month) // compare with shortend date
		date = date.addYears(1);
	datetime.setDate({
		date.year(),
		_month,
		1 //always set to the first of the month. if a day was specified, that one is set after this
	});
}



YearTerm::YearTerm(int year, bool certain) :
	SubTerm{AbsoluteTimepoint, Year, certain},
	_year{year}
{}

std::pair<QSharedPointer<YearTerm>, int> YearTerm::parse(const QStringRef &expression)
{
	const auto prefix = QStringLiteral("(%1)?").arg(trList(YearPrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)?").arg(trList(YearSuffix).join(QLatin1Char('|')));

	QRegularExpression regex {
		QLatin1Char('^') + prefix + QStringLiteral("(-?\\d{4,})") + suffix + QStringLiteral("\\s*"),
		QRegularExpression::DontAutomaticallyOptimizeOption |
		QRegularExpression::CaseInsensitiveOption |
		QRegularExpression::UseUnicodePropertiesOption
	};
	auto match = regex.match(expression);
	if(match.hasMatch()) {
		bool ok = false;
		auto year = match.captured(2).toInt(&ok);
		if(ok) {
			return {
				QSharedPointer<YearTerm>::create(year, match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
				match.capturedLength(0)
			};
		}
	}

	return {};
}

void YearTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	Q_UNUSED(applyRelative)
	datetime.setDate({_year, 1, 1}); //set the year and reset month/date. They will be specified as needed
}



SequenceTerm::SequenceTerm(Sequence &&sequence, bool looped, bool certain) :
	SubTerm{looped ? LoopedTimeSpan : Timespan, [&]() {
		Scope scope = InvalidScope;
		for(auto it = sequence.constBegin(); it != sequence.constEnd(); ++it)
			scope |= it.key();
		return scope;
	}(), certain},
	_sequence{std::move(sequence)}
{}

std::pair<QSharedPointer<SequenceTerm>, int> SequenceTerm::parse(const QStringRef &expression)
{
	// get and prepare standard *fixes
	const auto prefix = QStringLiteral("(%1)?").arg(trList(SpanPrefix).join(QLatin1Char('|')));
	const auto suffix = QStringLiteral("(%1)").arg(trList(SpanSuffix).join(QLatin1Char('|')));
	const auto conjunctors = QStringLiteral("(%1)").arg(trList(SpanConjuction).join(QLatin1Char('|')));
	// prepare list of combos to try. can be {loop, suffix}, {prefix, loop} or {prefix, suffix}, but the first two only if a loop*fix is defined
	QVector<std::pair<QString, bool>> exprCombos;
	exprCombos.reserve(2);
	{
		const auto loopPrefix = trList(SpanLoopPrefix);
		if(!loopPrefix.isEmpty())
			exprCombos.append(std::make_pair(QStringLiteral("(%1)").arg(loopPrefix.join(QLatin1Char('|'))), true));
	}
	exprCombos.append(std::make_pair(prefix, false));

	// prepare lookup of span scopes
	QHash<QString, ScopeFlag> nameLookup;
	QString nameKey;
	{
		QStringList nameKeys;
		for(const auto &scopeInfo : {
				std::make_pair(SpanKeyMinute, Minute),
				std::make_pair(SpanKeyHour, Hour),
				std::make_pair(SpanKeyDay, Day),
				std::make_pair(SpanKeyWeek, Week),
				std::make_pair(SpanKeyMonth, Month),
				std::make_pair(SpanKeyYear, Year)
			}) {
			for(const auto &key : trList(scopeInfo.first, false, false)) {
				nameLookup.insert(key, scopeInfo.second);
				nameKeys.append(key);
			}
		}
		// sort by length to test the longest variants first
		std::sort(nameKeys.begin(), nameKeys.end(), [](const QString &lhs, const QString &rhs) {
			return lhs.size() > rhs.size();
		});
		// escape after sorting
		for(auto &key : nameKeys)
			key = QRegularExpression::escape(key);
		nameKey = nameKeys.join(QLatin1Char('|'));
	}

	for(const auto &loopCombo : exprCombos) {
		// check for the prefix
		QRegularExpression prefixRegex {
			QLatin1Char('^') + loopCombo.first,
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto prefixMatch = prefixRegex.match(expression);
		if(!prefixMatch.hasMatch())
			continue;
		auto hasPrefix = prefixMatch.capturedLength(1) > 0;

		// iterate through all "and" expressions
		auto offset = prefixMatch.capturedLength(0);
		QRegularExpression regex {
			QLatin1Char('^') +
			QStringLiteral("(?:(\\d+)\\s)%1").arg(loopCombo.second ? QString{QLatin1Char('?')} : QString{}) +
			QLatin1Char('(') + nameKey + QStringLiteral(")(?:") +
			conjunctors + QLatin1Char('|') + suffix + QStringLiteral(")?\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};

		Sequence sequence;
		forever {
			auto match = regex.match(expression.mid(offset));
			if(match.hasMatch()) {
				// get the scope
				auto scope = nameLookup.value(match.captured(2).toLower(), InvalidScope);
				if(scope == InvalidScope || sequence.contains(scope))
					break;
				// get the amount of days
				bool ok = false;
				int numDays;
				if(loopCombo.second && match.capturedLength(1) == 0) {
					ok = true;
					numDays = 1;
				} else
					numDays = match.captured(1).toInt(&ok);
				if(!ok)
					break;
				// add to sequence
				sequence.insert(scope, numDays);

				// check how to continue
				if(match.capturedLength(3) > 0) {//has found conjunction
					hasPrefix = true;
					offset += match.capturedLength(0);
					// continue in loop
				} else {
					return {
						QSharedPointer<SequenceTerm>::create(std::move(sequence),
															 loopCombo.second,
															 hasPrefix || match.capturedLength(4) > 0 ),
						offset + match.capturedLength(0)
					};
				}
			} else
				break;
		};
	}

	return {};
}

void SequenceTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	using namespace std::chrono;
	Q_UNUSED(applyRelative)
	for(auto it = _sequence.constBegin(); it != _sequence.constEnd(); ++it) {
		switch(it.key()) {
		case Minute:
			datetime = datetime.addSecs(duration_cast<seconds>(minutes{*it}).count());
			break;
		case Hour:
			datetime = datetime.addSecs(duration_cast<seconds>(hours{*it}).count());
			break;
		case Day:
			datetime = datetime.addDays(*it);
			break;
		case Week:
			datetime = datetime.addDays(static_cast<qint64>(*it) * 7ll);
			break;
		case Month:
			datetime = datetime.addMonths(*it);
			break;
		case Year:
			datetime = datetime.addYears(*it);
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}
}



KeywordTerm::KeywordTerm(int days) :
	SubTerm{Timespan, Day, true},
	_days{days}
{}

std::pair<QSharedPointer<KeywordTerm>, int> KeywordTerm::parse(const QStringRef &expression)
{
	for(const auto &info : trList(KeywordDayspan, false)) {
		const auto split = info.split(QLatin1Char(':'));
		Q_ASSERT_X(split.size() == 2, Q_FUNC_INFO, "Invalid KeywordDayspan translation. Must be keyword and value, seperated by a ':'");
		QRegularExpression regex {
			QLatin1Char('^') + QRegularExpression::escape(split[0]) + QStringLiteral("\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = regex.match(expression);
		if(match.hasMatch()) {
			return {
				QSharedPointer<KeywordTerm>::create(split[1].toInt()),
				match.capturedLength(0)
			};
		}
	}

	return {};
}

void KeywordTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	Q_UNUSED(applyRelative)
	datetime = datetime.addDays(_days);
}



LimiterTerm::LimiterTerm(bool isFrom) :
	SubTerm{isFrom ? FromSubterm : UntilSubTerm, InvalidScope, true}
{}

std::pair<QSharedPointer<LimiterTerm>, int> LimiterTerm::parse(const QStringRef &expression)
{
	for(const auto &type : {std::make_pair(LimiterFromPrefix, true), std::make_pair(LimiterUntilPrefix, false)}) {
		QRegularExpression regex {
			QLatin1Char('^') + QStringLiteral("(?:%1)").arg(trList(type.first).join(QLatin1Char('|'))) + QStringLiteral("\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = regex.match(expression);
		if(match.hasMatch()) {
			return {
				QSharedPointer<LimiterTerm>::create(type.second),
				match.capturedLength(0)
			};
		}
	}

	return {};
}

void LimiterTerm::apply(QDateTime &datetime, bool applyRelative) const
{
	Q_UNUSED(applyRelative)
	datetime = _limitTerm.apply(datetime);
}



Term::Term(std::initializer_list<QSharedPointer<SubTerm> > args) :
	QList{args}
{}

Scope Term::scope() const
{
	Q_ASSERT(_scope != InvalidScope);
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

QDateTime Term::apply(QDateTime datetime) const
{
	auto applyFirst = true;
	for(const auto &term : *this) {
		if(term->type.testFlag(FlagLimiter)) // skip limiters when applying
			continue;
		term->apply(datetime, applyFirst);
		applyFirst = false;
	}
	return datetime;
}

void Term::finalize()
{
	Q_ASSERT(_scope == InvalidScope);
	for(const auto &subTerm : qAsConst(*this)) {
		_scope |= subTerm->scope;
		_looped = _looped || subTerm->type.testFlag(FlagLooped);
		_absolute = _absolute || subTerm->type.testFlag(FlagAbsolute);
	}
}




QString Expressions::trWord(WordKey key, bool escape)
{
	QString word;
	switch(key) {
	case TimePrefix:
		word = EventExpressionParser::tr("at ", "TimePrefix");
		break;
	case TimeSuffix:
		word = EventExpressionParser::tr(" o'clock", "TimeSuffix");
		break;
	case TimePattern:
		word = EventExpressionParser::tr("hh:mm ap|h:mm ap|hh:m ap|h:m ap|hh ap|h ap|"
										 "hh:mm AP|h:mm AP|hh:m AP|h:m AP|hh AP|h AP|"
										 "hh:mm|h:mm|hh:m|h:m|hh|h", "TimePattern");
		break;
	case DatePrefix:
		word = EventExpressionParser::tr("on |on the |the ", "DatePrefix");
		break;
	case DateSuffix:
		word = EventExpressionParser::tr("", "DateSuffix");
		break;
	case DatePattern:
		word= EventExpressionParser::tr("dd.MM.yyyy|d.MM.yyyy|dd.M.yyyy|d.M.yyyy|"
										"dd. MM. yyyy|d. MM. yyyy|dd. M. yyyy|d. M. yyyy|"
										"dd-MM-yyyy|d-MM-yyyy|dd-M-yyyy|d-M-yyyy|"

										"dd.MM.yy|d.MM.yy|dd.M.yy|d.M.yy|"
										"dd. MM. yy|d. MM. yy|dd. M. yy|d. M. yy|"
										"dd-MM-yy|d-MM-yy|dd-M-yy|d-M-yy|"

										"dd.MM.|d.MM.|dd.M.|d.M.|"
										"dd. MM.|d. MM.|dd. M.|d. M.|"
										"dd-MM|d-MM|dd-M|d-M", "DatePattern");
		break;
	case InvTimeExprPattern:
		word = EventExpressionParser::tr("%2 past %1:+|%2-past %1:+|%2 to %1:-", "InvTimeExprPattern");
		break;
	case Expressions::InvTimeHourPattern:
		word = EventExpressionParser::tr("hh ap|h ap|hh AP|h AP|hh|h", "InvTimeHourPattern");
		break;
	case Expressions::InvTimeMinutePattern:
		word = EventExpressionParser::tr("mm|m", "InvTimeMinutePattern");
		break;
	case InvTimeKeyword:
		word = EventExpressionParser::tr("quarter:15|half:30", "InvTimeKeywords");
		break;
	case MonthDayPrefix:
		word = EventExpressionParser::tr("on |on the |the ", "MonthDayPrefix");
		break;
	case MonthDaySuffix:
		word = EventExpressionParser::tr(" of", "MonthDaySuffix");
		break;
	case MonthDayLoopPrefix:
		word = EventExpressionParser::tr("every |any |all", "MonthDayLoopPrefix");
		break;
	case MonthDayLoopSuffix:
		word = EventExpressionParser::tr("", "MonthDayLoopSuffix");
		break;
	case MonthDayIndicator:
		word = EventExpressionParser::tr("_.|_th|_st|_nd|_rd", "MonthDayIndicator");
		break;
	case WeekDayPrefix:
		word = EventExpressionParser::tr("on ", "WeekDayPrefix");
		break;
	case WeekDaySuffix:
		word = EventExpressionParser::tr("", "WeekDaySuffix");
		break;
	case WeekDayLoopPrefix:
		word = EventExpressionParser::tr("every |any |all", "WeekDayLoopPrefix");
		break;
	case WeekDayLoopSuffix:
		word = EventExpressionParser::tr("", "WeekDayLoopSuffix");
		break;
	case MonthPrefix:
		word = EventExpressionParser::tr("in |on ", "MonthPrefix");
		break;
	case MonthSuffix:
		word = EventExpressionParser::tr("", "MonthSuffix");
		break;
	case MonthLoopPrefix:
		word = EventExpressionParser::tr("every |any |all", "MonthLoopPrefix");
		break;
	case MonthLoopSuffix:
		word = EventExpressionParser::tr("", "MonthLoopSuffix");
		break;
	case YearPrefix:
		word = EventExpressionParser::tr("in ", "YearPrefix");
		break;
	case YearSuffix:
		word = EventExpressionParser::tr("", "YearSuffix");
		break;
	case SpanPrefix:
		word = EventExpressionParser::tr("in ", "SpanPrefix");
		break;
	case SpanSuffix:
		word = EventExpressionParser::tr("", "SpanSuffix");
		break;
	case SpanLoopPrefix:
		word = EventExpressionParser::tr("every |all", "SpanLoopPrefix");
		break;
	case SpanConjuction:
		word = EventExpressionParser::tr(" and", "SpanConjuction");
		break;
	case SpanKeyMinute:
		word = EventExpressionParser::tr("min|mins|minute|minutes", "SpanKeyMinute");
		break;
	case SpanKeyHour:
		word = EventExpressionParser::tr("hour|hours", "SpanKeyHour");
		break;
	case SpanKeyDay:
		word = EventExpressionParser::tr("day|days", "SpanKeyDay");
		break;
	case SpanKeyWeek:
		word = EventExpressionParser::tr("week|weeks", "SpanKeyWeek");
		break;
	case SpanKeyMonth:
		word = EventExpressionParser::tr("mon|mons|month|months", "SpanKeyMonth");
		break;
	case SpanKeyYear:
		word = EventExpressionParser::tr("year|years", "SpanKeyYear");
		break;
	case KeywordDayspan:
		word = EventExpressionParser::tr("today:0|tomorrow:1", "KeywordDayspan");
		break;
	case LimiterFromPrefix:
		word = EventExpressionParser::tr("from", "LimiterFromPrefix");
		break;
	case LimiterUntilPrefix:
		word = EventExpressionParser::tr("until|to", "LimiterUntilPrefix");
		break;
	case ExpressionSeperator:
		word = EventExpressionParser::tr(";", "ExpressionSeperator");
		break;
	}
	if(escape)
		word = QRegularExpression::escape(word);
	return word;
}

QStringList Expressions::trList(WordKey key, bool escape, bool sort)
{
	auto resList = trWord(key, false).split(QLatin1Char('|'), QString::SkipEmptyParts);
	if(sort) {
		std::sort(resList.begin(), resList.end(), [](const QString &lhs, const QString &rhs) {
			return lhs.size() > rhs.size();
		});
	}
	if(escape) {
		for(auto &word : resList)
			word = QRegularExpression::escape(word);
	}
	return resList;
}

QString Expressions::dateTimeFormatToRegex(QString pattern, const std::function<void (QString &)> &replacer)
{
	Q_ASSERT(replacer);
	pattern = QRegularExpression::escape(pattern);
	auto afterQuote = 0;
	bool inQuote = false;
	forever {
		const auto nextQuote = pattern.indexOf(QStringLiteral("\\'"), afterQuote); // \' because already regex escaped
		auto replLen = nextQuote == -1 ?
						   pattern.size() - afterQuote :
						   nextQuote - afterQuote;

		if(!inQuote) {
			//special case - quote after quote
			if(nextQuote == afterQuote) {
				afterQuote += 2;
				inQuote = true;
				continue;
			}

			if(replLen > 0) {
				auto subStr = pattern.mid(afterQuote, replLen);
				replacer(subStr);
				pattern.replace(afterQuote, replLen, subStr);
				if(nextQuote != -1) {
					afterQuote += subStr.size() + 2;
					Q_ASSERT(pattern.midRef(afterQuote - 2, 2) == QStringLiteral("\\'"));
				}
			}
		} else if(nextQuote != -1) {
			pattern.replace(afterQuote - 2, replLen + 4, pattern.mid(afterQuote, replLen));
			afterQuote = nextQuote - 2;
		}

		if(nextQuote < 0)
			break;
		inQuote = !inQuote;
	};

	return pattern;
}
