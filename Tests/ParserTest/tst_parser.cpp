#include <QtTest>
#define private public
#define protected public
#include <eventexpressionparser.h>
#undef protected
#undef private
using namespace Expressions;

Q_DECLARE_METATYPE(Expressions::Type)
Q_DECLARE_METATYPE(Expressions::Scope)

class ParserTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testTimeRegexConversion_data();
	void testTimeRegexConversion();
	void testDateRegexConversion_data();
	void testDateRegexConversion();

	void testTimeExpressions_data();
	void testTimeExpressions();
	void testDateExpressions_data();
	void testDateExpressions();

private:
	EventExpressionParser *parser;
};

void ParserTest::initTestCase()
{
	QLocale::setDefault(QLocale::c());
	parser = new EventExpressionParser{this};
}

void ParserTest::cleanupTestCase()
{
	parser->deleteLater();
}

void ParserTest::testTimeRegexConversion_data()
{
	QTest::addColumn<QString>("pattern");
	QTest::addColumn<QString>("regex");

	QTest::addRow("simple") << QStringLiteral("hh:mm")
							<< QStringLiteral(R"__(\d{2}\:\d{2})__");
	QTest::addRow("variable_len") << QStringLiteral("h:m ap")
								  << QStringLiteral(R"__(\d{1,2}\:\d{1,2}\ (?:AM|PM))__");
	QTest::addRow("full") << QStringLiteral("hh:h:mm:m:ss:s:zzz:z:AP:ap")
						  << QStringLiteral(R"__(\d{2}\:\d{1,2}\:\d{2}\:\d{1,2}\:\d{2}\:\d{1,2}\:\d{3}\:\d{1,3}\:(?:AM|PM)\:(?:AM|PM))__");
	QTest::addRow("quoted.fully") << QStringLiteral("'hh:mm'")
								  << QStringLiteral(R"__(hh\:mm)__");
	QTest::addRow("quoted.begin") << QStringLiteral("'hh:'mm")
								  << QStringLiteral(R"__(hh\:\d{2})__");
	QTest::addRow("quoted.end") << QStringLiteral("hh:'mm'")
								<< QStringLiteral(R"__(\d{2}\:mm)__");
	QTest::addRow("quoted.mid") << QStringLiteral("h'h:m'm")
								  << QStringLiteral(R"__(\d{1,2}h\:m\d{1,2})__");
	QTest::addRow("quoted.outer") << QStringLiteral("'h'h:m'm'")
								  << QStringLiteral(R"__(h\d{1,2}\:\d{1,2}m)__");
	QTest::addRow("quoted.empty.text") << QStringLiteral("hh'':mm")
									   << QStringLiteral(R"__(\d{2}\:\d{2})__");
	QTest::addRow("quoted.empty.pattern") << QStringLiteral("'hh'':mm'")
										  << QStringLiteral(R"__(hh\:mm)__");
	QTest::addRow("quoted.empty.insane") << QStringLiteral("''''''''''''")
										 << QString{};
	QTest::addRow("quoted.single") << QStringLiteral("h o'clock")
								   << QStringLiteral(R"__(\d{1,2}\ o\'clock)__");
}

void ParserTest::testTimeRegexConversion()
{
	QFETCH(QString, pattern);
	QFETCH(QString, regex);

	QCOMPARE(TimeTerm::toRegex(pattern), regex);
}

void ParserTest::testDateRegexConversion_data()
{
	QTest::addColumn<QString>("pattern");
	QTest::addColumn<QString>("regex");
	QTest::addColumn<bool>("hasYear");

	QTest::addRow("simple.month") << QStringLiteral("dd. MM.")
								  << QStringLiteral(R"__(\d{2}\.\ \d{2}\.)__")
								  << false;
	QTest::addRow("simple.year") << QStringLiteral("d-M-yyyy")
								 << QStringLiteral(R"__(\d{1,2}\-\d{1,2}\--?\d{4})__")
								 << true;
	QTest::addRow("dslashed") << QStringLiteral(R"__(d \d dd \dd \d)__")
							  << QStringLiteral(R"__(\d{1,2}\ \\\d{1,2}\ \d{2}\ \\\d{2}\ \\\d{1,2})__")
							  << false;
	QTest::addRow("full") << QStringLiteral("yyyy:yy:MM:M:dd:d")
						  << QStringLiteral(R"__(-?\d{4}\:\d{2}\:\d{2}\:\d{1,2}\:\d{2}\:\d{1,2})__")
						  << true;
	QTest::addRow("quoted.fully") << QStringLiteral("'dd.MM.'")
								  << QStringLiteral(R"__(dd\.MM\.)__")
								  << false;
	QTest::addRow("quoted.begin") << QStringLiteral("'dd.'MM.")
								  << QStringLiteral(R"__(dd\.\d{2}\.)__")
								  << false;
	QTest::addRow("quoted.end") << QStringLiteral("dd.'MM.'")
								<< QStringLiteral(R"__(\d{2}\.MM\.)__")
								<< false;
	QTest::addRow("quoted.mid") << QStringLiteral("d'd.M'M.")
								  << QStringLiteral(R"__(\d{1,2}d\.M\d{1,2}\.)__")
								  << false;
	QTest::addRow("quoted.outer") << QStringLiteral("'d'd.M'M.'")
								  << QStringLiteral(R"__(d\d{1,2}\.\d{1,2}M\.)__")
								  << false;
	QTest::addRow("quoted.empty.text") << QStringLiteral("dd.''MM.")
									   << QStringLiteral(R"__(\d{2}\.\d{2}\.)__")
									   << false;
	QTest::addRow("quoted.empty.pattern") << QStringLiteral("'dd.''MM.'")
										  << QStringLiteral(R"__(dd\.MM\.)__")
										  << false;
	QTest::addRow("quoted.empty.insane") << QStringLiteral("''''''''''''")
										 << QString{}
										 << false;
	QTest::addRow("quoted.single") << QStringLiteral("d o'day")
								   << QStringLiteral(R"__(\d{1,2}\ o\'day)__")
								   << false;
}

void ParserTest::testDateRegexConversion()
{
	QFETCH(QString, pattern);
	QFETCH(QString, regex);
	QFETCH(bool, hasYear);

	bool year = !hasYear;
	QCOMPARE(DateTerm::toRegex(pattern, year), regex);
	QCOMPARE(year, hasYear);
}

void ParserTest::testTimeExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<QTime>("time");
	QTest::addColumn<int>("offset");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	QTest::addRow("simple") << QStringLiteral("14:00")
							<< QTime{14, 00}
							<< 5
							<< false
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate::currentDate(), QTime{14, 00}};
	QTest::addRow("prefix") << QStringLiteral("at 7:00")
							<< QTime{7, 00}
							<< 7
							<< true
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate::currentDate(), QTime{7, 00}};
	QTest::addRow("suffix") << QStringLiteral("14:5 o'clock")
							<< QTime{14, 5}
							<< 12
							<< true
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate::currentDate(), QTime{14, 5}};
	QTest::addRow("allfix") << QStringLiteral("at 7 o'clock")
							<< QTime{7, 0}
							<< 12
							<< true
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate::currentDate(), QTime{7, 0}};

	// with remaing
	QTest::addRow("substr") << QStringLiteral("05 cars")
							<< QTime{5, 00}
							<< 3
							<< false
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate::currentDate(), QTime{5, 00}};
	QTest::addRow("pmstr") << QStringLiteral("3 pm cars")
						   << QTime{15, 00}
						   << 5
						   << false
						   << QDateTime::currentDateTime()
						   << QDateTime{QDate::currentDate(), QTime{15, 00}};

	// invalid
	QTest::addRow("partial") << QStringLiteral("14:30:25 pm")
							 << QTime{14, 30}
							 << 5
							 << false
							 << QDateTime::currentDateTime()
							 << QDateTime{QDate::currentDate(), QTime{14, 30}};
	QTest::addRow("partial.rest") << QStringLiteral(":25 pm")
								  << QTime{}
								  << 0
								  << false
								  << QDateTime{}
								  << QDateTime{};
	QTest::addRow("invalid") << QStringLiteral("60:25")
							 << QTime{}
							 << 0
							 << false
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testTimeExpressions()
{
	QFETCH(QString, expression);
	QFETCH(QTime, time);
	QFETCH(int, offset);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = TimeTerm::parse(expression.midRef(0)); //pass full str
	if(time.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, RelativeTimepoint);
		QCOMPARE(res.first->scope, Hour | Minute);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_time, time);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testDateExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<QDate>("date");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<Scope>("scope");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	auto cYear = QDate::currentDate().year();
	QTest::addRow("simple.dots") << QStringLiteral("13.12.")
								 << QDate{1900, 12, 13}
								 << 6
								 << Type{RelativeTimepoint}
								 << Scope{Month | MonthDay}
								 << false
								 << QDateTime::currentDateTime()
								 << QDateTime{QDate{cYear, 12, 13}, QTime::currentTime()};
	QTest::addRow("simple.dash") << QStringLiteral("5-3")
								  << QDate{1900, 3, 5}
								  << 3
								  << Type{RelativeTimepoint}
								  << Scope{Month | MonthDay}
								  << false
								  << QDateTime::currentDateTime()
								  << QDateTime{QDate{cYear, 3, 5}, QTime::currentTime()};
	QTest::addRow("year.dots") << QStringLiteral("3.3.95")
							   << QDate{1995, 3, 3}
							   << 6
							   << Type{AbsoluteTimepoint}
							   << Scope{Year | Month | MonthDay}
							   << false
							   << QDateTime::currentDateTime()
							   << QDateTime{QDate{1995, 3, 3}, QTime::currentTime()};
	QTest::addRow("year.dash") << QStringLiteral("25-10-2010")
							   << QDate{2010, 10, 25}
							   << 10
							   << Type{AbsoluteTimepoint}
							   << Scope{Year | Month | MonthDay}
							   << false
							   << QDateTime::currentDateTime()
							   << QDateTime{QDate{2010, 10, 25}, QTime::currentTime()};
	QTest::addRow("prefix.simple") << QStringLiteral("on 11. 11.")
								   << QDate{1900, 11, 11}
								   << 10
								   << Type{RelativeTimepoint}
								   << Scope{Month | MonthDay}
								   << true
								   << QDateTime::currentDateTime()
								   << QDateTime{QDate{cYear, 11, 11}, QTime::currentTime()};
	QTest::addRow("prefix.year") << QStringLiteral("on 2. 1. 2014")
								 << QDate{2014, 1, 2}
								 << 13
								 << Type{AbsoluteTimepoint}
								 << Scope{Year | Month | MonthDay}
								 << true
								 << QDateTime::currentDateTime()
								 << QDateTime{QDate{2014, 1, 2}, QTime::currentTime()};
	QTest::addRow("substr") << QStringLiteral("on 24-12 at 14:00")
							<< QDate{1900, 12, 24}
							<< 9
							<< Type{RelativeTimepoint}
							<< Scope{Month | MonthDay}
							<< true
							<< QDateTime::currentDateTime()
							<< QDateTime{QDate{cYear, 12, 24}, QTime::currentTime()};

	// invalid
	QTest::addRow("partial") << QStringLiteral("24. 12. 03.")
							 << QDate{1903, 12, 24}
							 << 10
							 << Type{AbsoluteTimepoint}
							 << Scope{Year | Month | MonthDay}
							 << false
							 << QDateTime::currentDateTime()
							 << QDateTime{QDate{1903, 12, 24}, QTime::currentTime()};
	QTest::addRow("invalid") << QStringLiteral("10.")
							 << QDate{}
							 << 0
							 << Type{InvalidType}
							 << Scope{InvalidScope}
							 << false
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testDateExpressions()
{
	QFETCH(QString, expression);
	QFETCH(QDate, date);
	QFETCH(int, offset);
	QFETCH(Type, type);
	QFETCH(Scope, scope);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = DateTerm::parse(expression.midRef(0)); //pass full str
	if(date.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, scope);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_date, date);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

QTEST_MAIN(ParserTest)

#include "tst_parser.moc"
