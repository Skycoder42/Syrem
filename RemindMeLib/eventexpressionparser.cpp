#include "eventexpressionparser.h"

#include <QLocale>
using namespace Expressions;

EventExpressionParser::EventExpressionParser(QObject *parent) :
	QObject{parent}
{}





SubTerm::~SubTerm() = default;


std::pair<QSharedPointer<TimeTerm>, int> TimeTerm::parse(const QStringRef &expression)
{
	QLocale locale;
	auto prefixList = trList(TimePrefix);
	auto suffixList = trList(TimeSuffix);
	auto patterns = trList(TimePattern, false);

	auto prefix = prefixList.isEmpty() ?
					  QString{} :
					  QStringLiteral("(%1\\s)?").arg(prefixList.join(QLatin1Char('|')));
	auto suffix = suffixList.isEmpty() ?
					  QString{} :
					  QStringLiteral("(\\s%1)?").arg(suffixList.join(QLatin1Char('|')));
	for(const auto &pattern : patterns) {
		QRegularExpression regex {
			QLatin1Char('^') + prefix + QLatin1Char('(') + toRegex(pattern) + QLatin1Char(')') + suffix + QStringLiteral("\\s*"),
			QRegularExpression::DontAutomaticallyOptimizeOption |
			QRegularExpression::CaseInsensitiveOption |
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = regex.match(expression);
		if(match.hasMatch()) {
			auto subLen = match.capturedLength(0);
			auto time = locale.toTime(match.captured(2), pattern);
			if(time.isValid()) {
				return {
					QSharedPointer<TimeTerm>::create(time,
													 match.capturedLength(1) > 0 || match.capturedLength(3) > 0 ),
					subLen
				};
			}
		}
	}

	return {};
}

void TimeTerm::apply(QDateTime &datetime)
{
	datetime.setTime(_time);
}

QString TimeTerm::toRegex(QString pattern)
{
	pattern = QRegularExpression::escape(pattern);
	QLocale locale;
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
				auto subStr = pattern.mid(afterQuote, replLen)
							  .replace(QStringLiteral("hh"), QStringLiteral("\\d{2}"))
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



QString Expressions::trWord(WordKey key, bool escape)
{
	QString word;
	switch (key) {
	case Expressions::TimePrefix:
		word = EventExpressionParser::tr("at");
		break;
	case Expressions::TimeSuffix:
		word = EventExpressionParser::tr("o'clock");
		break;
	case Expressions::TimePattern:
		word = EventExpressionParser::tr("hh:mm ap|h:mm ap|hh:m ap|h:m ap|hh ap|h ap|"
										 "hh:mm AP|h:mm AP|hh:m AP|h:m AP|hh AP|h AP|"
										 "hh:mm|h:mm|hh:m|h:m|hh|h");
		break;
	}
	if(escape)
		word = QRegularExpression::escape(word);
	return word;
}

QStringList Expressions::trList(WordKey key, bool escape)
{
	auto resList = trWord(key, false).split(QLatin1Char('|'));
	if(escape) {
		for(auto &word : resList)
			word = QRegularExpression::escape(word);
	}
	return resList;
}
