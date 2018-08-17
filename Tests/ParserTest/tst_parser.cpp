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
	void testInvertedTimeExpressions_data();
	void testInvertedTimeExpressions();
	void testMonthDayExpressions_data();
	void testMonthDayExpressions();
	void testMonthExpressions_data();
	void testMonthExpressions();

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
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("simple") << QStringLiteral("14:00")
							<< QTime{14, 00}
							<< 5
							<< false
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 00}};
	QTest::addRow("prefix") << QStringLiteral("at 7:00")
							<< QTime{7, 00}
							<< 7
							<< true
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{7, 00}};
	QTest::addRow("suffix") << QStringLiteral("14:5 o'clock")
							<< QTime{14, 5}
							<< 12
							<< true
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 5}};
	QTest::addRow("allfix") << QStringLiteral("at 7 o'clock")
							<< QTime{7, 0}
							<< 12
							<< true
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{7, 0}};

	// with remaing
	QTest::addRow("substr") << QStringLiteral("05 cars")
							<< QTime{5, 00}
							<< 3
							<< false
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{5, 00}};
	QTest::addRow("pmstr") << QStringLiteral("3 pm cars")
						   << QTime{15, 00}
						   << 5
						   << false
						   << QDateTime{cDate, cTime}
						   << QDateTime{cDate, QTime{15, 00}};

	// invalid
	QTest::addRow("partial") << QStringLiteral("14:30:25 pm")
							 << QTime{14, 30}
							 << 5
							 << false
							 << QDateTime{cDate, cTime}
							 << QDateTime{cDate, QTime{14, 30}};
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
		res.first->apply(since, true);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testDateExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<QDate>("date");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<Scope>("scope");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	auto cYear = cDate.year();
	QTest::addRow("simple.dots") << QStringLiteral("13.12.")
								 << false
								 << QDate{1900, 12, 13}
								 << 6
								 << Type{RelativeTimepoint}
								 << Scope{Month | MonthDay}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 12, 13}, cTime};
	QTest::addRow("simple.dash") << QStringLiteral("5-3")
								 << false
								 << QDate{1900, 3, 5}
								 << 3
								 << Type{RelativeTimepoint}
								 << Scope{Month | MonthDay}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 3, 5}, cTime};
	QTest::addRow("year.dots") << QStringLiteral("3.3.95")
							   << false
							   << QDate{1995, 3, 3}
							   << 6
							   << Type{AbsoluteTimepoint}
							   << Scope{Year | Month | MonthDay}
							   << false
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{1995, 3, 3}, cTime};
	QTest::addRow("year.dash") << QStringLiteral("25-10-2010")
							   << false
							   << QDate{2010, 10, 25}
							   << 10
							   << Type{AbsoluteTimepoint}
							   << Scope{Year | Month | MonthDay}
							   << false
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{2010, 10, 25}, cTime};
	QTest::addRow("prefix.simple") << QStringLiteral("on 11. 11.")
								   << false
								   << QDate{1900, 11, 11}
								   << 10
								   << Type{RelativeTimepoint}
								   << Scope{Month | MonthDay}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 11, 11}, cTime};
	QTest::addRow("prefix.year") << QStringLiteral("on 2. 1. 2014")
								 << false
								 << QDate{2014, 1, 2}
								 << 13
								 << Type{AbsoluteTimepoint}
								 << Scope{Year | Month | MonthDay}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{2014, 1, 2}, cTime};
	QTest::addRow("substr") << QStringLiteral("on 24-12 at 14:00")
							<< false
							<< QDate{1900, 12, 24}
							<< 9
							<< Type{RelativeTimepoint}
							<< Scope{Month | MonthDay}
							<< true
							<< QDateTime{cDate, cTime}
							<< QDateTime{QDate{cYear, 12, 24}, cTime};

	QDateTime referenceTime{{2018, 10, 10}, {14, 0}};
	cYear = referenceTime.date().year();
	QTest::addRow("offset.future.noKeep") << QStringLiteral("13.12.")
										  << false
										  << QDate{1900, 12, 13}
										  << 6
										  << Type{RelativeTimepoint}
										  << Scope{Month | MonthDay}
										  << false
										  << referenceTime
										  << QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.future.keep") << QStringLiteral("13.12.")
										<< true
										<< QDate{1900, 12, 13}
										<< 6
										<< Type{RelativeTimepoint}
										<< Scope{Month | MonthDay}
										<< false
										<< referenceTime
										<< QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("15.07.")
										<< false
										<< QDate{1900, 7, 15}
										<< 6
										<< Type{RelativeTimepoint}
										<< Scope{Month | MonthDay}
										<< false
										<< referenceTime
										<< QDateTime{QDate{cYear, 7, 15}, referenceTime.time()};
	QTest::addRow("offset.past.keep") << QStringLiteral("15.07.")
									  << true
									  << QDate{1900, 7, 15}
									  << 6
									  << Type{RelativeTimepoint}
									  << Scope{Month | MonthDay}
									  << false
									  << referenceTime
									  << QDateTime{QDate{cYear + 1, 7, 15}, referenceTime.time()};

	// invalid
	QTest::addRow("partial") << QStringLiteral("24. 12. 03.")
							 << false
							 << QDate{1903, 12, 24}
							 << 10
							 << Type{AbsoluteTimepoint}
							 << Scope{Year | Month | MonthDay}
							 << false
							 << QDateTime{cDate, cTime}
							 << QDateTime{QDate{1903, 12, 24}, cTime};
	QTest::addRow("invalid") << QStringLiteral("10.")
							 << false
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
	QFETCH(bool, applyRelative);
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
		res.first->apply(since, applyRelative);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testInvertedTimeExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<QTime>("time");
	QTest::addColumn<int>("offset");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("numbered.past") << QStringLiteral("10 past 11")
								   << QTime{11, 10}
								   << 10
								   << QDateTime{cDate, cTime}
								   << QDateTime{cDate, QTime{11, 10}};
	QTest::addRow("numbered.to") << QStringLiteral("at 4 to 3 pm")
								 << QTime{14, 56}
								 << 12
								 << QDateTime{cDate, cTime}
								 << QDateTime{cDate, QTime{14, 56}};
	QTest::addRow("quarter.past") << QStringLiteral("at quarter past 17")
								  << QTime{17, 15}
								  << 18
								  << QDateTime{cDate, cTime}
								  << QDateTime{cDate, QTime{17, 15}};
	QTest::addRow("quarter.to") << QStringLiteral("quarter to 12")
								<< QTime{11, 45}
								<< 13
								<< QDateTime{cDate, cTime}
								<< QDateTime{cDate, QTime{11, 45}};
	QTest::addRow("half.past") << QStringLiteral("half-past 7")
							   << QTime{7, 30}
							   << 11
							   << QDateTime{cDate, cTime}
							   << QDateTime{cDate, QTime{7, 30}};

	QTest::addRow("substr") << QStringLiteral("at 4 past 14 am")
							<< QTime{14, 4}
							<< 13
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 4}};
	QTest::addRow("invalid.text") << QStringLiteral("at car past 12")
								  << QTime{}
								  << 0
								  << QDateTime{}
								  << QDateTime{};
	QTest::addRow("invalid.offset") << QStringLiteral("70 to 4")
								  << QTime{}
								  << 0
								  << QDateTime{}
								  << QDateTime{};
}

void ParserTest::testInvertedTimeExpressions()
{
	QFETCH(QString, expression);
	QFETCH(QTime, time);
	QFETCH(int, offset);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = InvertedTimeTerm::parse(expression.midRef(0)); //pass full str
	if(time.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, RelativeTimepoint);
		QCOMPARE(res.first->scope, Hour | Minute);
		QCOMPARE(res.first->certain, true);
		QCOMPARE(res.first->_time, time);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, true);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testMonthDayExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<int>("day");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	auto cYear = cDate.year();
	auto cMonth = cDate.month();
	QTest::addRow("simple.raw") << QStringLiteral("12.")
								<< false
								<< 12
								<< 3
								<< Type{RelativeTimepoint}
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, cMonth, 12}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("the 23rd")
								   << false
								   << 23
								   << 8
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 23}, cTime};
	QTest::addRow("simple.suffix") << QStringLiteral("2nd of")
								   << false
								   << 2
								   << 6
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 2}, cTime};
	QTest::addRow("simple.allfix") << QStringLiteral("on the 1st of")
								   << false
								   << 1
								   << 13
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 1}, cTime};

	QTest::addRow("boundary.valid") << QStringLiteral("31.")
									<< false
									<< 31
									<< 3
									<< Type{RelativeTimepoint}
									<< false
									<< QDateTime{QDate{cYear, 5, 1}, cTime}
									<< QDateTime{QDate{cYear, 5, 31}, cTime};
	QTest::addRow("boundary.monthover") << QStringLiteral("31.")
										<< false
										<< 31
										<< 3
										<< Type{RelativeTimepoint}
										<< false
										<< QDateTime{QDate{cYear, 4, 1}, cTime}
										<< QDateTime{QDate{cYear, 4, 30}, cTime};
	QTest::addRow("boundary.overflow") << QStringLiteral("40.")
									   << false
									   << 0
									   << 0
									   << Type{InvalidType}
									   << false
									   << QDateTime{}
									   << QDateTime{};
	QTest::addRow("boundary.underflow") << QStringLiteral("0.")
										<< false
										<< 0
										<< 0
										<< Type{InvalidType}
										<< false
										<< QDateTime{}
										<< QDateTime{};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("14.")
										  << false
										  << 14
										  << 3
										  << Type{RelativeTimepoint}
										  << false
										  << QDateTime{QDate{cYear, cMonth, 10}, cTime}
										  << QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("14.")
										<< true
										<< 14
										<< 3
										<< Type{RelativeTimepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 10}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("14.")
										<< false
										<< 14
										<< 3
										<< Type{RelativeTimepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 20}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("14.")
									  << true
									  << 14
									  << 3
									  << Type{RelativeTimepoint}
									  << false
									  << QDateTime{QDate{cYear, cMonth, 20}, cTime}
									  << QDateTime{QDate{cYear, cMonth + 1, 14}, cTime};
	QTest::addRow("offset.monthover.noKeep") << QStringLiteral("30.")
											 << false
											 << 30
											 << 3
											 << Type{RelativeTimepoint}
											 << false
											 << QDateTime{QDate{cYear, 2, 28}, cTime}
											 << QDateTime{QDate{cYear, 2, 28}, cTime};
	QTest::addRow("offset.monthover.keep") << QStringLiteral("30.")
										   << true
										   << 30
										   << 3
										   << Type{RelativeTimepoint}
										   << false
										   << QDateTime{QDate{cYear, 2, 28}, cTime}
										   << QDateTime{QDate{cYear, 3, 30}, cTime};

	QTest::addRow("loop.simple") << QStringLiteral("every 21st")
								 << false
								 << 21
								 << 10
								 << Type{LoopedTimePoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 21}, cTime};
	QTest::addRow("loop.long") << QStringLiteral("every 5. of")
							   << false
							   << 5
							   << 11
							   << Type{LoopedTimePoint}
							   << true
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{cYear, cMonth, 5}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("on 4th of july")
								   << false
								   << 4
								   << 10
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 4}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every 3. in June")
								 << false
								 << 3
								 << 9
								 << Type{LoopedTimePoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 3}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in 10")
							 << false
							 << 0
							 << 0
							 << Type{InvalidScope}
							 << false
							 << QDateTime{}
							 << QDateTime{};

}

void ParserTest::testMonthDayExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, day);
	QFETCH(int, offset);
	QFETCH(Type, type);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = MonthDayTerm::parse(expression.midRef(0)); //pass full str
	if(day > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, MonthDay);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_day, day);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, applyRelative);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testMonthExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<int>("month");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	auto cYear = cDate.year();
	QTest::addRow("simple.raw") << QStringLiteral("May")
								<< false
								<< 5
								<< 3
								<< Type{RelativeTimepoint}
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, 5, 1}, cTime};
	QTest::addRow("simple.short") << QStringLiteral("Dec")
								  << false
								  << 12
								  << 3
								  << Type{RelativeTimepoint}
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{cYear, 12, 1}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("October")
								 << false
								 << 10
								 << 7
								 << Type{RelativeTimepoint}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 10, 1}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in June")
								   << false
								   << 6
								   << 7
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 6, 1}, cTime};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("April")
										  << false
										  << 4
										  << 5
										  << Type{RelativeTimepoint}
										  << false
										  << QDateTime{QDate{cYear, 2, 10}, cTime}
										  << QDateTime{QDate{cYear, 4, 1}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("July")
										<< true
										<< 7
										<< 4
										<< Type{RelativeTimepoint}
										<< false
										<< QDateTime{QDate{cYear, 5, 10}, cTime}
										<< QDateTime{QDate{cYear, 7, 1}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("Feb")
										<< false
										<< 2
										<< 3
										<< Type{RelativeTimepoint}
										<< false
										<< QDateTime{QDate{cYear, 7, 20}, cTime}
										<< QDateTime{QDate{cYear, 2, 1}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("March")
									  << true
									  << 3
									  << 5
									  << Type{RelativeTimepoint}
									  << false
									  << QDateTime{QDate{cYear, 7, 20}, cTime}
									  << QDateTime{QDate{cYear + 1, 3, 1}, cTime};

	QTest::addRow("loop.short") << QStringLiteral("every Nov")
								<< false
								<< 11
								<< 9
								<< Type{LoopedTimePoint}
								<< true
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, 11, 1}, cTime};
	QTest::addRow("loop.long") << QStringLiteral("every August")
							   << false
							   << 8
							   << 12
							   << Type{LoopedTimePoint}
							   << true
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{cYear, 8, 1}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("in September on 27th")
								   << false
								   << 9
								   << 13
								   << Type{RelativeTimepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 9, 1}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every September on 27th")
								 << false
								 << 9
								 << 16
								 << Type{LoopedTimePoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 9, 1}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("in Octobear")
								 << false
								 << 10
								 << 6
								 << Type{RelativeTimepoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 10, 1}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in Jarnurary")
							 << false
							 << 0
							 << 0
							 << Type{InvalidScope}
							 << false
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testMonthExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, month);
	QFETCH(int, offset);
	QFETCH(Type, type);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = MonthTerm::parse(expression.midRef(0)); //pass full str
	if(month > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, Month);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_month, month);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, applyRelative);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

QTEST_MAIN(ParserTest)

#include "tst_parser.moc"
