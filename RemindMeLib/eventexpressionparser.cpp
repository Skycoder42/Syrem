#include "eventexpressionparser.h"

#include <QLocale>
#include <QVector>
using namespace Expressions;

EventExpressionParser::EventExpressionParser(QObject *parent) :
	QObject{parent}
{}





SubTerm::~SubTerm() = default;



TimeTerm::TimeTerm(QTime time, bool certain) :
	SubTerm{RelativeTimepoint, Hour | Minute, certain},
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
	SubTerm{hasYear ? AbsoluteTimepoint : RelativeTimepoint,
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
	SubTerm{RelativeTimepoint, Hour | Minute, true},
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
	SubTerm{looped ? LoopedTimePoint : RelativeTimepoint, MonthDay, certain},
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
					auto isLoop =  std::get<2>(loopCombo);
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
	}
	if(escape)
		word = QRegularExpression::escape(word);
	return word;
}

QStringList Expressions::trList(WordKey key, bool escape)
{
	auto resList = trWord(key, false).split(QLatin1Char('|'), QString::SkipEmptyParts);
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
