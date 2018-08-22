#include <QtTest>
#include <QtMvvmCore/ServiceRegistry>
#include <QtDataSync/Setup>
#define private public
#define protected public
#include <eventexpressionparser.h>
#undef protected
#undef private
using namespace Expressions;

Q_DECLARE_METATYPE(Expressions::Type)
Q_DECLARE_METATYPE(Expressions::Scope)
Q_DECLARE_METATYPE(Expressions::SequenceTerm::Sequence)

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
	void testWeekDayExpressions_data();
	void testWeekDayExpressions();
	void testMonthExpressions_data();
	void testMonthExpressions();
	void testYearExpressions_data();
	void testYearExpressions();
	void testSequenceExpressions_data();
	void testSequenceExpressions();
	void testKeywordExpressions_data();
	void testKeywordExpressions();

	void testExpressionParsing_data();
	void testExpressionParsing();
	void testMultiExpressionParsing_data();
	void testMultiExpressionParsing();
	void testExpressionLimiters_data();
	void testExpressionLimiters();

	void testTermEvaluation_data();
	void testTermEvaluation();

private:
	QTemporaryDir tDir;
	EventExpressionParser *parser;
};

void ParserTest::initTestCase()
{
	QLocale::setDefault(QLocale::c());

	QtDataSync::Setup()
			.setLocalDir(tDir.path())
			.create();

	parser = QtMvvm::ServiceRegistry::instance()->constructInjected<EventExpressionParser>(this);
}

void ParserTest::cleanupTestCase()
{
	delete parser;
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
		QCOMPARE(res.first->type, Timepoint);
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
								 << Type{Timepoint}
								 << Scope{Month | MonthDay}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 12, 13}, cTime};
	QTest::addRow("simple.dash") << QStringLiteral("5-3")
								 << false
								 << QDate{1900, 3, 5}
								 << 3
								 << Type{Timepoint}
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
								   << Type{Timepoint}
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
							<< Type{Timepoint}
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
										  << Type{Timepoint}
										  << Scope{Month | MonthDay}
										  << false
										  << referenceTime
										  << QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.future.keep") << QStringLiteral("13.12.")
										<< true
										<< QDate{1900, 12, 13}
										<< 6
										<< Type{Timepoint}
										<< Scope{Month | MonthDay}
										<< false
										<< referenceTime
										<< QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("15.07.")
										<< false
										<< QDate{1900, 7, 15}
										<< 6
										<< Type{Timepoint}
										<< Scope{Month | MonthDay}
										<< false
										<< referenceTime
										<< QDateTime{QDate{cYear, 7, 15}, referenceTime.time()};
	QTest::addRow("offset.past.keep") << QStringLiteral("15.07.")
									  << true
									  << QDate{1900, 7, 15}
									  << 6
									  << Type{Timepoint}
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
		QCOMPARE(res.first->type, Timepoint);
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
	const auto cYear = cDate.year();
	const auto cMonth = cDate.month();
	QTest::addRow("simple.raw") << QStringLiteral("12.")
								<< false
								<< 12
								<< 3
								<< Type{Timepoint}
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, cMonth, 12}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("the 23rd")
								   << false
								   << 23
								   << 8
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 23}, cTime};
	QTest::addRow("simple.suffix") << QStringLiteral("2nd of")
								   << false
								   << 2
								   << 6
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 2}, cTime};
	QTest::addRow("simple.allfix") << QStringLiteral("on the 1st of")
								   << false
								   << 1
								   << 13
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 1}, cTime};

	QTest::addRow("boundary.valid") << QStringLiteral("31.")
									<< false
									<< 31
									<< 3
									<< Type{Timepoint}
									<< false
									<< QDateTime{QDate{cYear, 5, 1}, cTime}
									<< QDateTime{QDate{cYear, 5, 31}, cTime};
	QTest::addRow("boundary.monthover") << QStringLiteral("31.")
										<< false
										<< 31
										<< 3
										<< Type{Timepoint}
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
										  << Type{Timepoint}
										  << false
										  << QDateTime{QDate{cYear, cMonth, 10}, cTime}
										  << QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("14.")
										<< true
										<< 14
										<< 3
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 10}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("14.")
										<< false
										<< 14
										<< 3
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 20}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("14.")
									  << true
									  << 14
									  << 3
									  << Type{Timepoint}
									  << false
									  << QDateTime{QDate{cYear, cMonth, 20}, cTime}
									  << QDateTime{QDate{cYear, cMonth + 1, 14}, cTime};
	QTest::addRow("offset.monthover.noKeep") << QStringLiteral("30.")
											 << false
											 << 30
											 << 3
											 << Type{Timepoint}
											 << false
											 << QDateTime{QDate{cYear, 2, 28}, cTime}
											 << QDateTime{QDate{cYear, 2, 28}, cTime};
	QTest::addRow("offset.monthover.keep") << QStringLiteral("30.")
										   << true
										   << 30
										   << 3
										   << Type{Timepoint}
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
								   << Type{Timepoint}
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

void ParserTest::testWeekDayExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<int>("weekDay");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const QDate cDate{2018, 10, 15}; //somewhere in the middle of a month
	const auto cTime = QTime::currentTime();
	const auto cYear = cDate.year();
	const auto cMonth = cDate.month();
	QTest::addRow("simple.raw") << QStringLiteral("Thursday")
								<< false
								<< 4
								<< 8
								<< Type{Timepoint}
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, cMonth, 18}, cTime};
	QTest::addRow("simple.short") << QStringLiteral("Fri")
								  << false
								  << 5
								  << 3
								  << Type{Timepoint}
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{cYear, cMonth, 19}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("Monday")
								 << false
								 << 1
								 << 6
								 << Type{Timepoint}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("on Wed")
								   << false
								   << 3
								   << 6
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 17}, cTime};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("Sat")
										  << false
										  << 6
										  << 3
										  << Type{Timepoint}
										  << false
										  << QDateTime{QDate{cYear, cMonth, 17}, cTime}
										  << QDateTime{QDate{cYear, cMonth, 20}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("Sunday")
										<< true
										<< 7
										<< 6
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 17}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 21}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("Monday")
										<< false
										<< 1
										<< 6
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, cMonth, 17}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("Tue")
									  << true
									  << 2
									  << 3
									  << Type{Timepoint}
									  << false
									  << QDateTime{QDate{cYear, cMonth, 17}, cTime}
									  << QDateTime{QDate{cYear, cMonth, 23}, cTime};
	QTest::addRow("offset.boundary.noKeep") << QStringLiteral("Wed")
											<< false
											<< 3
											<< 3
											<< Type{Timepoint}
											<< false
											<< QDateTime{QDate{cYear, 12, 28}, cTime}
											<< QDateTime{QDate{cYear, 12, 26}, cTime};
	QTest::addRow("offset.boundary.keep") << QStringLiteral("Wed")
										  << true
										  << 3
										  << 3
										  << Type{Timepoint}
										  << false
										  << QDateTime{QDate{cYear, 12, 28}, cTime}
										  << QDateTime{QDate{cYear + 1, 1, 2}, cTime};

	QTest::addRow("loop") << QStringLiteral("every Friday")
						  << false
						  << 5
						  << 12
						  << Type{LoopedTimePoint}
						  << true
						  << QDateTime{cDate, cTime}
						  << QDateTime{QDate{cYear, cMonth, 19}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("on Monday next week")
								   << false
								   << 1
								   << 10
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every Wed in June")
								 << false
								 << 3
								 << 10
								 << Type{LoopedTimePoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 17}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("on Weddingday")
								 << false
								 << 3
								 << 6
								 << Type{Timepoint}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 17}, cTime};
	QTest::addRow("invalid") << QStringLiteral("on Thorsday")
							 << false
							 << 0
							 << 0
							 << Type{InvalidScope}
							 << false
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testWeekDayExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, weekDay);
	QFETCH(int, offset);
	QFETCH(Type, type);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = WeekDayTerm::parse(expression.midRef(0)); //pass full str
	if(weekDay > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, WeekDay);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_weekDay, weekDay);
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
	const auto cYear = cDate.year();
	QTest::addRow("simple.raw") << QStringLiteral("May")
								<< false
								<< 5
								<< 3
								<< Type{Timepoint}
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, 5, 1}, cTime};
	QTest::addRow("simple.short") << QStringLiteral("Dec")
								  << false
								  << 12
								  << 3
								  << Type{Timepoint}
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{cYear, 12, 1}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("October")
								 << false
								 << 10
								 << 7
								 << Type{Timepoint}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 10, 1}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in June")
								   << false
								   << 6
								   << 7
								   << Type{Timepoint}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 6, 1}, cTime};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("April")
										  << false
										  << 4
										  << 5
										  << Type{Timepoint}
										  << false
										  << QDateTime{QDate{cYear, 2, 10}, cTime}
										  << QDateTime{QDate{cYear, 4, 1}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("July")
										<< true
										<< 7
										<< 4
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, 5, 10}, cTime}
										<< QDateTime{QDate{cYear, 7, 1}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("Feb")
										<< false
										<< 2
										<< 3
										<< Type{Timepoint}
										<< false
										<< QDateTime{QDate{cYear, 7, 20}, cTime}
										<< QDateTime{QDate{cYear, 2, 1}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("March")
									  << true
									  << 3
									  << 5
									  << Type{Timepoint}
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
								   << Type{Timepoint}
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
								 << Type{Timepoint}
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

void ParserTest::testYearExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<int>("year");
	QTest::addColumn<bool>("valid");
	QTest::addColumn<int>("offset");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("simple.short") << QStringLiteral("2015")
								  << 2015
								  << true
								  << 4
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{2015, 1, 1}, cTime};
	QTest::addRow("simple.low") << QStringLiteral("0045")
								<< 45
								<< true
								<< 4
								<< false
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{45, 1, 1}, cTime};
	QTest::addRow("simple.negative") << QStringLiteral("-2015")
									 << -2015
									 << true
									 << 5
									 << false
									 << QDateTime{cDate, cTime}
									 << QDateTime{QDate{-2015, 1, 1}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("124563")
								 << 124563
								 << true
								 << 6
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{124563, 1, 1}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in 0000")
								   << 0
								   << true
								   << 7
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{0, 1, 1}, cTime};

	QTest::addRow("substr") << QStringLiteral("in 2017 in June")
							<< 2017
							<< true
							<< 8
							<< true
							<< QDateTime{cDate, cTime}
							<< QDateTime{QDate{2017, 1, 1}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in 95")
							 << 0
							 << false
							 << 0
							 << false
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testYearExpressions()
{
	QFETCH(QString, expression);
	QFETCH(int, year);
	QFETCH(bool, valid);
	QFETCH(int, offset);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = YearTerm::parse(expression.midRef(0)); //pass full str
	if(valid) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, AbsoluteTimepoint);
		QCOMPARE(res.first->scope, Year);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_year, year);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, true);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testSequenceExpressions_data()
{
	using Seq = SequenceTerm::Sequence;

	QTest::addColumn<QString>("expression");
	QTest::addColumn<SequenceTerm::Sequence>("sequence");
	QTest::addColumn<int>("offset");
	QTest::addColumn<Type>("type");
	QTest::addColumn<Scope>("scope");
	QTest::addColumn<bool>("certain");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const QDate cDate{2018, 7, 12};
	const QTime cTime{14, 30};
	const auto cYear = cDate.year();
	const auto cMonth = cDate.month();
	const auto cDay = cDate.day();
	const auto cHour = cTime.hour();
	const auto cMin = cTime.minute();
	QTest::addRow("simple.minutes") << QStringLiteral("10 mins")
									<< Seq{{Minute, 10}}
									<< 7
									<< Type{Timespan}
									<< Scope{Minute}
									<< false
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour, cMin + 10}};
	QTest::addRow("simple.hours") << QStringLiteral("5 hours")
								  << Seq{{Hour, 5}}
								  << 7
								  << Type{Timespan}
								  << Scope{Hour}
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{cDate, {cHour + 5, cMin}};
	QTest::addRow("simple.days") << QStringLiteral("1 day")
								 << Seq{{Day, 1}}
								 << 5
								 << Type{Timespan}
								 << Scope{Day}
								 << false
								 << QDateTime{cDate, cTime}
								 << QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("simple.weeks") << QStringLiteral("2 weeks")
								  << Seq{{Week, 2}}
								  << 7
								  << Type{Timespan}
								  << Scope{Week}
								  << false
								  << QDateTime{cDate, cTime}
								  << QDateTime{{cYear, cMonth, cDay + 14}, cTime};
	QTest::addRow("simple.months") << QStringLiteral("4 months")
								   << Seq{{Month, 4}}
								   << 8
								   << Type{Timespan}
								   << Scope{Month}
								   << false
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth + 4, cDay}, cTime};
	QTest::addRow("simple.years") << QStringLiteral("40 years")
								   << Seq{{Year, 40}}
								   << 8
								   << Type{Timespan}
								   << Scope{Year}
								   << false
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear + 40, cMonth, cDay}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in 3 days")
								   << Seq{{Day, 3}}
								   << 9
								   << Type{Timespan}
								   << Scope{Day}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth, cDay + 3}, cTime};

	QTest::addRow("combined.times") << QStringLiteral("3 hours and 50 minutes")
									<< Seq{{Hour, 3}, {Minute, 50}}
									<< 22
									<< Type{Timespan}
									<< Scope{Minute | Hour}
									<< true
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour + 4, 20}};
	QTest::addRow("combined.dates") << QStringLiteral("24 days and 5 years and 10 mons")
									<< Seq{{Day, 24}, {Year, 5}, {Month, 10}}
									<< 31
									<< Type{Timespan}
									<< Scope{Year | Month | Day}
									<< true
									<< QDateTime{cDate, cTime}
									<< QDateTime{{2024, 6, 5}, cTime};
	QTest::addRow("combined.all") << QStringLiteral("1 week and 2 mins and 3 years and 4 hours and 5 months and 6 days")
								  << Seq{{Week, 1}, {Minute, 2}, {Year, 3}, {Hour, 4}, {Month, 5}, {Day, 6}}
								  << 65
								  << Type{Timespan}
								  << Scope{Year | Month | Week | Day | Hour | Minute}
								  << true
								  << QDateTime{cDate, cTime}
								  << QDateTime{{cYear + 3, cMonth + 5, cDay + 13}, {cHour + 4, cMin + 2}};
	QTest::addRow("combined.prefix") << QStringLiteral("in 2 weeks and 30 mins")
									 << Seq{{Week, 2}, {Minute, 30}}
									 << 22
									 << Type{Timespan}
									 << Scope{Minute | Week}
									 << true
									 << QDateTime{cDate, cTime}
									 << QDateTime{{cYear, cMonth, cDay + 14}, {cHour + 1, 00}};

	QTest::addRow("loop.simple") << QStringLiteral("every 3 weeks")
								 << Seq{{Week, 3}}
								 << 13
								 << Type{LoopedTimeSpan}
								 << Scope{Week}
								 << true
								 << QDateTime{cDate, cTime}
								 << QDateTime{{cYear, cMonth + 1, 2}, cTime};
	QTest::addRow("loop.singular") << QStringLiteral("every day")
								   << Seq{{Day, 1}}
								   << 9
								   << Type{LoopedTimeSpan}
								   << Scope{Day}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("loop.combined") << QStringLiteral("every 3 hours and 2 years and week")
								   << Seq{{Hour, 3}, {Year, 2}, {Week, 1}}
								   << 34
								   << Type{LoopedTimeSpan}
								   << Scope{Hour | Year | Week}
								   << true
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear + 2, cMonth, cDay + 7}, {cHour + 3, cMin}};

	QTest::addRow("substr.simple") << QStringLiteral("in 10 mins in 3 days")
									<< Seq{{Minute, 10}}
									<< 11
									<< Type{Timespan}
									<< Scope{Minute}
									<< true
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour, cMin + 10}};
	QTest::addRow("substr.loop") << QStringLiteral("every day of the year")
									<< Seq{{Day, 1}}
									<< 10
									<< Type{LoopedTimeSpan}
									<< Scope{Day}
									<< true
									<< QDateTime{cDate, cTime}
									<< QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("3 dayz")
									<< Seq{{Day, 3}}
									<< 5
									<< Type{Timespan}
									<< Scope{Day}
									<< false
									<< QDateTime{cDate, cTime}
									<< QDateTime{{cYear, cMonth, cDay + 3}, cTime};

	QTest::addRow("invalid.format") << QStringLiteral("in 10 horas")
									<< Seq{}
									<< 0
									<< Type{InvalidType}
									<< Scope{InvalidScope}
									<< false
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.number") << QStringLiteral("in day")
									<< Seq{}
									<< 0
									<< Type{InvalidType}
									<< Scope{InvalidScope}
									<< false
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.combined") << QStringLiteral("in 2 mins and 5 minutes")
									  << Seq{}
									  << 0
									  << Type{InvalidType}
									  << Scope{InvalidScope}
									  << false
									  << QDateTime{}
									  << QDateTime{};
	QTest::addRow("invalid.substr") << QStringLiteral("in 10 mins and in 3 days")
									<< Seq{}
									<< 0
									<< Type{InvalidType}
									<< Scope{InvalidScope}
									<< false
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.loop") << QStringLiteral("every 10 horas")
								  << Seq{}
								  << 0
								  << Type{InvalidType}
								  << Scope{InvalidScope}
								  << false
								  << QDateTime{}
								  << QDateTime{};
}

void ParserTest::testSequenceExpressions()
{
	QFETCH(QString, expression);
	QFETCH(SequenceTerm::Sequence, sequence);
	QFETCH(int, offset);
	QFETCH(Type, type);
	QFETCH(Scope, scope);
	QFETCH(bool, certain);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = SequenceTerm::parse(expression.midRef(0)); //pass full str
	if(!sequence.isEmpty()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, scope);
		QCOMPARE(res.first->certain, certain);
		QCOMPARE(res.first->_sequence, sequence);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, true);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testKeywordExpressions_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<int>("days");
	QTest::addColumn<int>("offset");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("today") << QStringLiteral("today")
						   << 0
						   << 5
						   << QDateTime{cDate, cTime}
						   << QDateTime{cDate, cTime};
	QTest::addRow("tomorrow") << QStringLiteral("tomorrow ")
							  << 1
							  << 9
							  << QDateTime{cDate, cTime}
							  << QDateTime{cDate.addDays(1), cTime};
	QTest::addRow("invalid") << QStringLiteral("invalid")
							 << -1
							 << 0
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testKeywordExpressions()
{
	QFETCH(QString, expression);
	QFETCH(int, days);
	QFETCH(int, offset);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = KeywordTerm::parse(expression.midRef(0)); //pass full str
	if(days >= 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, Timespan);
		QCOMPARE(res.first->scope, Day);
		QCOMPARE(res.first->certain, true);
		QCOMPARE(res.first->_days, days);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, true);
		QCOMPARE(since, result);
	} else
		QVERIFY(!res.first);
}

void ParserTest::testExpressionParsing_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<int>("depth");
	QTest::addColumn<Scope>("scope");
	QTest::addColumn<bool>("looped");
	QTest::addColumn<bool>("absolute");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("time") << QStringLiteral("14:00")
						  << 1
						  << Scope{Hour | Minute}
						  << false
						  << false
						  << QDateTime{cDate, cTime}
						  << QDateTime{cDate, {14, 0}};
	QTest::addRow("date") << QStringLiteral("March 24th")
						  << 2
						  << Scope{MonthDay | Month}
						  << false
						  << false
						  << QDateTime{{2018, 2, 10}, cTime}
						  << QDateTime{{2018, 3, 24}, cTime};
	QTest::addRow("datetime") << QStringLiteral("on March the 24th at 14:00")
							  << 3
							  << Scope{MonthDay | Month | Hour | Minute}
							  << false
							  << false
							  << QDateTime{{2018, 2, 10}, cTime}
							  << QDateTime{{2018, 3, 24}, {14, 0}};

	QTest::addRow("parts.time") << QStringLiteral("in 3 hours 20 mins")
								<< 2
								<< Scope{Hour | Minute}
								<< false
								<< false
								<< QDateTime{cDate, {14, 10}}
								<< QDateTime{cDate, {17, 30}};
	QTest::addRow("parts.date") << QStringLiteral("every 2 years and 3 months on Saturday")
								<< 2
								<< Scope{Year | Month | WeekDay}
								<< true
								<< false
								<< QDateTime{{2018, 2, 10}, cTime}
								<< QDateTime{{2020, 5, 9}, cTime};
	QTest::addRow("parts.datetime") << QStringLiteral("in 2019 on 24.10. at quarter past 10")
									<< 3
									<< Scope{Year | Month | MonthDay | Hour | Minute}
									<< false
									<< true
									<< QDateTime{{2010, 11, 11}, cTime}
									<< QDateTime{{2019, 10, 24}, {10, 15}};

	QTest::addRow("invalid.scope") << QStringLiteral("in April at 4 o'clock on 24.12.")
								   << 0
								   << Scope{InvalidScope}
								   << false
								   << false
								   << QDateTime{}
								   << QDateTime{};
	QTest::addRow("invalid.loop") << QStringLiteral("every 3 days every 20 minutes")
								  << 0
								  << Scope{InvalidScope}
								  << false
								  << false
								  << QDateTime{}
								  << QDateTime{};
	QTest::addRow("invalid.pointspan") << QStringLiteral("In November in 3 weeks")
									   << 0
									   << Scope{InvalidScope}
									   << false
									   << false
									   << QDateTime{}
									   << QDateTime{};
	QTest::addRow("loop.subscope") << QStringLiteral("In November every week")
								   << 2
								   << Scope{Month | Week}
								   << true
								   << false
								   << QDateTime{{2018, 7, 15}, cTime}
								   << QDateTime{{2018, 11, 8}, cTime};
}

void ParserTest::testExpressionParsing()
{
	QFETCH(QString, expression);
	QFETCH(int, depth);
	QFETCH(Scope, scope);
	QFETCH(bool, looped);
	QFETCH(bool, absolute);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	auto terms = parser->parseExpression(expression);
	if(depth == 0)
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
	else {
		QEXPECT_FAIL("loop.subscope", "Not implemented yet", Abort);

		QCOMPARE(terms.size(), 1);
		auto &term = terms.first();
		QCOMPARE(term.size(), depth);
		QCOMPARE(term.scope(), scope);
		QCOMPARE(term.isLooped(), looped);
		QCOMPARE(term.isAbsolute(), absolute);
		auto date = term.apply(since);
		QCOMPARE(date, result);
	}
}

void ParserTest::testMultiExpressionParsing_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<int>("count");
	QTest::addColumn<QList<QDateTime>>("since");
	QTest::addColumn<QList<QDateTime>>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("singular") << QStringLiteral("in 10 days at 14:30")
							  << 1
							  << QList<QDateTime>{{{2018, 12, 13}, cTime}}
							  << QList<QDateTime>{{{2018, 12, 23}, {14, 30}}};
	QTest::addRow("dual") << QStringLiteral("in 10 days; at 14:30")
						  << 2
						  << QList<QDateTime>{{{2018, 12, 13}, cTime}, {cDate, cTime}}
						  << QList<QDateTime>{{{2018, 12, 23}, cTime}, {cDate, {14, 30}}};
	QTest::addRow("quintuple") << QStringLiteral("in 10 days; at 14:30 ;in 2020 ; tomorrow;10 to 11")
							   << 5
							   << QList<QDateTime>{
										{{2018, 12, 13}, cTime},
										{cDate, cTime},
										{cDate, cTime},
										{{2018, 12, 13}, cTime},
										{cDate, cTime}
									}
							   << QList<QDateTime>{
										{{2018, 12, 23}, cTime},
										{cDate, {14, 30}},
										{{2020, 1, 1}, cTime},
										{{2018, 12, 14}, cTime},
										{cDate, {10, 50}},
									};
}

void ParserTest::testMultiExpressionParsing()
{
	QFETCH(QString, expression);
	QFETCH(int, count);
	QFETCH(QList<QDateTime>, since);
	QFETCH(QList<QDateTime>, result);

	Q_ASSERT(since.size() == count);
	Q_ASSERT(result.size() == count);

	auto terms = parser->parseMultiExpression(expression);
	QCOMPARE(terms.size(), count);
	for(auto i = 0; i < count; i++) {
		QCOMPARE(terms[i].size(), 1);
		auto date = terms[i].first().apply(since[i]);
		QCOMPARE(date, result[i]);
	}
}

void ParserTest::testExpressionLimiters_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<bool>("valid");
	QTest::addColumn<QDateTime>("sinceFrom");
	QTest::addColumn<QDateTime>("resultFrom");
	QTest::addColumn<QDateTime>("sinceUntil");
	QTest::addColumn<QDateTime>("resultUntil");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("loop.from") << QStringLiteral("every day from 10. of June")
							   << true
							   << QDateTime{{2017, 10, 16}, cTime}
							   << QDateTime{{2018, 6, 10}, cTime}
							   << QDateTime{}
							   << QDateTime{};
	QTest::addRow("loop.until") << QStringLiteral("every Tuesday until 2020")
								<< true
								<< QDateTime{}
								<< QDateTime{}
								<< QDateTime{{2018, 1, 2}, cTime}
								<< QDateTime{{2020, 1, 1}, cTime};
//	QTest::addRow("loop.from-until") << QStringLiteral("every 20 minutes from 10 to 14")
//									 << true
//									 << QDateTime{{2017, 10, 16}, cTime}
//									 << QDateTime{{2018, 6, 10}, cTime}
//									 << QDateTime{}
//									 << QDateTime{}; //TODO use to test for duplicates
	QTest::addRow("loop.from-until") << QStringLiteral("every 20 minutes from 10:00 to 17:15")
									 << true
									 << QDateTime{cDate, {8, 0}}
									 << QDateTime{cDate, {10, 0}}
									 << QDateTime{cDate, {8, 0}}
									 << QDateTime{cDate, {17, 15}};
	QTest::addRow("loop.until-from") << QStringLiteral("every year until June from 2015")
									 << true
									 << QDateTime{{2010, 7, 15}, cTime}
									 << QDateTime{{2015, 1, 1}, cTime}
									 << QDateTime{{2018, 4, 5}, cTime}
									 << QDateTime{{2018, 6, 1}, cTime};

	QTest::addRow("invalid.noloop.from") << QStringLiteral("tomorrow from 7:00")
										 << false
										 << QDateTime{}
										 << QDateTime{}
										 << QDateTime{}
										 << QDateTime{};
	QTest::addRow("invalid.noloop.until") << QStringLiteral("on 24.10. to 2020")
										  << false
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{};
	QTest::addRow("invalid.double.from") << QStringLiteral("every day from 10. of June from 10:00")
										 << false
										 << QDateTime{}
										 << QDateTime{}
										 << QDateTime{}
										 << QDateTime{};
	QTest::addRow("invalid.double.until") << QStringLiteral("every Tuesday until 2020 to 17:15")
										  << false
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{};
}

void ParserTest::testExpressionLimiters()
{
	QFETCH(QString, expression);
	QFETCH(bool, valid);
	QFETCH(QDateTime, sinceFrom);
	QFETCH(QDateTime, resultFrom);
	QFETCH(QDateTime, sinceUntil);
	QFETCH(QDateTime, resultUntil);

	auto terms = parser->parseExpression(expression);
	if(valid) {
		QCOMPARE(terms.size(), 1);
		auto &term = terms.first();
		for(const auto &subTerm : term) {
			if(subTerm->type == FromSubterm) {
				QVERIFY2(sinceFrom.isValid(), "Found unexpected from term");
				QVERIFY(subTerm.dynamicCast<LimiterTerm>());
				subTerm->apply(sinceFrom, true);
				QCOMPARE(sinceFrom, resultFrom);
				sinceFrom = QDateTime{};
			}
			if(subTerm->type == UntilSubTerm) {
				QVERIFY2(sinceUntil.isValid(), "Found unexpected until term");
				QVERIFY(subTerm.dynamicCast<LimiterTerm>());
				subTerm->apply(sinceUntil, true);
				QCOMPARE(sinceUntil, resultUntil);
				sinceUntil = QDateTime{};
			}
		}

		QVERIFY2(!sinceFrom.isValid(), "Expected from term but none was found");
		QVERIFY2(!sinceUntil.isValid(), "Expected until term but none was found");
	} else
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
}

void ParserTest::testTermEvaluation_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<QTime>("parserTime");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("valid.time") << QStringLiteral("in 2 days at 15:30")
								<< QTime{9, 0}
								<< QDateTime{cDate, cTime}
								<< QDateTime{cDate.addDays(2), {15, 30}};
	QTest::addRow("valid.autotime") << QStringLiteral("in 5 months")
									<< QTime{9, 0}
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate.addMonths(5), {9, 0}};
	QTest::addRow("valid.notime") << QStringLiteral("in August the 10th")
								  << QTime{0, 0}
								  << QDateTime{{2018, 4, 27}, cTime}
								  << QDateTime{{2018, 8, 10}, cTime};
	QTest::addRow("invalid.past") << QStringLiteral("11.09.2015 at half past 3 am")
								  << QTime{}
								  << QDateTime{cDate, cTime}
								  << QDateTime{};
}

void ParserTest::testTermEvaluation()
{
	QFETCH(QString, expression);
	QFETCH(QTime, parserTime);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	parser->_settings->scheduler.defaultTime = parserTime;
	auto terms = parser->parseExpression(expression);
	if(!since.isValid())
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
	else {
		QCOMPARE(terms.size(), 1);
		auto res = parser->evaluteTerm(terms.first(), since);
		QCOMPARE(res, result);
	}
}

QTEST_MAIN(ParserTest)

#include "tst_parser.moc"
