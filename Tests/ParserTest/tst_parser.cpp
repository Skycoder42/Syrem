#include <QtTest>
#include <QtMvvmCore>
#include <QtDataSync>
#define private public
#define protected public
#include <eventexpressionparser.h>
#include <terms.h>
#undef protected
#undef private
#include <schedule.h>
using namespace Expressions;

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
	void testTermSplitting_data();
	void testTermSplitting();

	void testTermEvaluation_data();
	void testTermEvaluation();
	void testSingularSchedules_data();
	void testSingularSchedules();
	void testRepeatedSchedules_data();
	void testRepeatedSchedules();

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
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<QTime>("time");
	QTest::addColumn<int>("offset");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("simple") << QStringLiteral("14:00")
							<< false
							<< QTime{14, 00}
							<< 5
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 00}};
	QTest::addRow("prefix") << QStringLiteral("at 7:00")
							<< false
							<< QTime{7, 00}
							<< 7
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{7, 00}};
	QTest::addRow("suffix") << QStringLiteral("14:5 o'clock")
							<< false
							<< QTime{14, 5}
							<< 12
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 5}};
	QTest::addRow("allfix") << QStringLiteral("at 7 o'clock")
							<< false
							<< QTime{7, 0}
							<< 12
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{7, 0}};

	// with remaing
	QTest::addRow("substr") << QStringLiteral("05 cars")
							<< false
							<< QTime{5, 00}
							<< 3
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{5, 00}};
	QTest::addRow("pmstr") << QStringLiteral("3 pm cars")
						   << false
						   << QTime{15, 00}
						   << 5
						   << QDateTime{cDate, cTime}
						   << QDateTime{cDate, QTime{15, 00}};

	QDateTime referenceTime{{2018, 10, 10}, {14, 0}};
	QTest::addRow("offset.future.noKeep") << QStringLiteral("17:30")
										  << false
										  << QTime{17, 30}
										  << 5
										  << referenceTime
										  << QDateTime{referenceTime.date(), {17, 30}};
	QTest::addRow("offset.future.keep") << QStringLiteral("17:30")
										<< true
										<< QTime{17, 30}
										<< 5
										<< referenceTime
										<< QDateTime{referenceTime.date(), {17, 30}};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("10:10")
										<< false
										<< QTime{10, 10}
										<< 5
										<< referenceTime
										<< QDateTime{referenceTime.date(), {10, 10}};
	QTest::addRow("offset.past.keep") << QStringLiteral("10:10")
									  << true
									  << QTime{10, 10}
									  << 5
									  << referenceTime
									  << QDateTime{referenceTime.date().addDays(1), {10, 10}};

	// invalid
	QTest::addRow("partial") << QStringLiteral("14:30:25 pm")
							 << false
							 << QTime{14, 30}
							 << 5
							 << QDateTime{cDate, cTime}
							 << QDateTime{cDate, QTime{14, 30}};
	QTest::addRow("partial.rest") << QStringLiteral(":25 pm")
								  << false
								  << QTime{}
								  << 0
								  << QDateTime{}
								  << QDateTime{};
	QTest::addRow("invalid") << QStringLiteral("60:25")
							 << false
							 << QTime{}
							 << 0
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testTimeExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(QTime, time);
	QFETCH(int, offset);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = TimeTerm::parse(expression.midRef(0)); //pass full str
	if(time.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, SubTerm::Timepoint);
		QCOMPARE(res.first->scope, SubTerm::Hour | SubTerm::Minute);
		QCOMPARE(res.first->_time, time);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, applyRelative);
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
	QTest::addColumn<SubTerm::Type>("type");
	QTest::addColumn<SubTerm::Scope>("scope");
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
								 << SubTerm::Type{SubTerm::Timepoint}
								 << SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 12, 13}, cTime};
	QTest::addRow("simple.dash") << QStringLiteral("5-3")
								 << false
								 << QDate{1900, 3, 5}
								 << 3
								 << SubTerm::Type{SubTerm::Timepoint}
								 << SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 3, 5}, cTime};
	QTest::addRow("year.dots") << QStringLiteral("3.3.95")
							   << false
							   << QDate{1995, 3, 3}
							   << 6
							   << SubTerm::Type{SubTerm::AbsoluteTimepoint}
							   << SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::MonthDay}
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{1995, 3, 3}, cTime};
	QTest::addRow("year.dash") << QStringLiteral("25-10-2010")
							   << false
							   << QDate{2010, 10, 25}
							   << 10
							   << SubTerm::Type{SubTerm::AbsoluteTimepoint}
							   << SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::MonthDay}
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{2010, 10, 25}, cTime};
	QTest::addRow("prefix.simple") << QStringLiteral("on 11. 11.")
								   << false
								   << QDate{1900, 11, 11}
								   << 10
								   << SubTerm::Type{SubTerm::Timepoint}
								   << SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 11, 11}, cTime};
	QTest::addRow("prefix.year") << QStringLiteral("on 2. 1. 2014")
								 << false
								 << QDate{2014, 1, 2}
								 << 13
								 << SubTerm::Type{SubTerm::AbsoluteTimepoint}
								 << SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::MonthDay}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{2014, 1, 2}, cTime};
	QTest::addRow("substr") << QStringLiteral("on 24-12 at 14:00")
							<< false
							<< QDate{1900, 12, 24}
							<< 9
							<< SubTerm::Type{SubTerm::Timepoint}
							<< SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
							<< QDateTime{cDate, cTime}
							<< QDateTime{QDate{cYear, 12, 24}, cTime};

	QDateTime referenceTime{{2018, 10, 10}, {14, 0}};
	cYear = referenceTime.date().year();
	QTest::addRow("offset.future.noKeep") << QStringLiteral("13.12.")
										  << false
										  << QDate{1900, 12, 13}
										  << 6
										  << SubTerm::Type{SubTerm::Timepoint}
										  << SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
										  << referenceTime
										  << QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.future.keep") << QStringLiteral("13.12.")
										<< true
										<< QDate{1900, 12, 13}
										<< 6
										<< SubTerm::Type{SubTerm::Timepoint}
										<< SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
										<< referenceTime
										<< QDateTime{QDate{cYear, 12, 13}, referenceTime.time()};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("15.07.")
										<< false
										<< QDate{1900, 7, 15}
										<< 6
										<< SubTerm::Type{SubTerm::Timepoint}
										<< SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
										<< referenceTime
										<< QDateTime{QDate{cYear, 7, 15}, referenceTime.time()};
	QTest::addRow("offset.past.keep") << QStringLiteral("15.07.")
									  << true
									  << QDate{1900, 7, 15}
									  << 6
									  << SubTerm::Type{SubTerm::Timepoint}
									  << SubTerm::Scope{SubTerm::Month | SubTerm::MonthDay}
									  << referenceTime
									  << QDateTime{QDate{cYear + 1, 7, 15}, referenceTime.time()};

	// invalid
	QTest::addRow("partial") << QStringLiteral("24. 12. 03.")
							 << false
							 << QDate{1903, 12, 24}
							 << 10
							 << SubTerm::Type{SubTerm::AbsoluteTimepoint}
							 << SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::MonthDay}
							 << QDateTime{cDate, cTime}
							 << QDateTime{QDate{1903, 12, 24}, cTime};
	QTest::addRow("invalid") << QStringLiteral("10.")
							 << false
							 << QDate{}
							 << 0
							 << SubTerm::Type{SubTerm::InvalidType}
							 << SubTerm::Scope{SubTerm::InvalidScope}
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testDateExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(QDate, date);
	QFETCH(int, offset);
	QFETCH(SubTerm::Type, type);
	QFETCH(SubTerm::Scope, scope);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = DateTerm::parse(expression.midRef(0)); //pass full str
	if(date.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, scope);
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
	QTest::addColumn<bool>("applyRelative");
	QTest::addColumn<QTime>("time");
	QTest::addColumn<int>("offset");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("numbered.past") << QStringLiteral("10 past 11")
								   << false
								   << QTime{11, 10}
								   << 10
								   << QDateTime{cDate, cTime}
								   << QDateTime{cDate, QTime{11, 10}};
	QTest::addRow("numbered.to") << QStringLiteral("at 4 to 3 pm")
								 << false
								 << QTime{14, 56}
								 << 12
								 << QDateTime{cDate, cTime}
								 << QDateTime{cDate, QTime{14, 56}};
	QTest::addRow("quarter.past") << QStringLiteral("at quarter past 17")
								  << false
								  << QTime{17, 15}
								  << 18
								  << QDateTime{cDate, cTime}
								  << QDateTime{cDate, QTime{17, 15}};
	QTest::addRow("quarter.to") << QStringLiteral("quarter to 12")
								<< false
								<< QTime{11, 45}
								<< 13
								<< QDateTime{cDate, cTime}
								<< QDateTime{cDate, QTime{11, 45}};
	QTest::addRow("half.past") << QStringLiteral("half-past 7")
							   << false
							   << QTime{7, 30}
							   << 11
							   << QDateTime{cDate, cTime}
							   << QDateTime{cDate, QTime{7, 30}};

	QDateTime referenceTime{{2018, 10, 10}, {14, 0}};
	QTest::addRow("offset.future.noKeep") << QStringLiteral("half past 5 pm")
										  << false
										  << QTime{17, 30}
										  << 14
										  << referenceTime
										  << QDateTime{referenceTime.date(), {17, 30}};
	QTest::addRow("offset.future.keep") << QStringLiteral("half past 5 pm")
										<< true
										<< QTime{17, 30}
										<< 14
										<< referenceTime
										<< QDateTime{referenceTime.date(), {17, 30}};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("quarter past 10")
										<< false
										<< QTime{10, 15}
										<< 15
										<< referenceTime
										<< QDateTime{referenceTime.date(), {10, 15}};
	QTest::addRow("offset.past.keep") << QStringLiteral("quarter past 10")
									  << true
									  << QTime{10, 15}
									  << 15
									  << referenceTime
									  << QDateTime{referenceTime.date().addDays(1), {10, 15}};

	QTest::addRow("substr") << QStringLiteral("at 4 past 14 am")
							<< false
							<< QTime{14, 4}
							<< 13
							<< QDateTime{cDate, cTime}
							<< QDateTime{cDate, QTime{14, 4}};
	QTest::addRow("invalid.text") << QStringLiteral("at car past 12")
								  << false
								  << QTime{}
								  << 0
								  << QDateTime{}
								  << QDateTime{};
	QTest::addRow("invalid.offset") << QStringLiteral("70 to 4")
									<< false
									<< QTime{}
									<< 0
									<< QDateTime{}
									<< QDateTime{};
}

void ParserTest::testInvertedTimeExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(QTime, time);
	QFETCH(int, offset);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = InvertedTimeTerm::parse(expression.midRef(0)); //pass full str
	if(time.isValid()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, SubTerm::Timepoint);
		QCOMPARE(res.first->scope, SubTerm::Hour | SubTerm::Minute);
		QCOMPARE(res.first->_time, time);
		QCOMPARE(res.second, offset);

		// second: test applying
		res.first->apply(since, applyRelative);
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
	QTest::addColumn<SubTerm::Type>("type");
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
								<< SubTerm::Type{SubTerm::Timepoint}
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, cMonth, 12}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("the 23rd")
								   << false
								   << 23
								   << 8
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 23}, cTime};
	QTest::addRow("simple.suffix") << QStringLiteral("2nd of")
								   << false
								   << 2
								   << 6
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 2}, cTime};
	QTest::addRow("simple.allfix") << QStringLiteral("on the 1st of")
								   << false
								   << 1
								   << 13
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 1}, cTime};

	QTest::addRow("boundary.valid") << QStringLiteral("31.")
									<< false
									<< 31
									<< 3
									<< SubTerm::Type{SubTerm::Timepoint}
									<< QDateTime{QDate{cYear, 5, 1}, cTime}
									<< QDateTime{QDate{cYear, 5, 31}, cTime};
	QTest::addRow("boundary.monthover") << QStringLiteral("31.")
										<< false
										<< 31
										<< 3
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, 4, 1}, cTime}
										<< QDateTime{QDate{cYear, 4, 30}, cTime};
	QTest::addRow("boundary.overflow") << QStringLiteral("40.")
									   << false
									   << 0
									   << 0
									   << SubTerm::Type{SubTerm::InvalidType}
									   << QDateTime{}
									   << QDateTime{};
	QTest::addRow("boundary.underflow") << QStringLiteral("0.")
										<< false
										<< 0
										<< 0
										<< SubTerm::Type{SubTerm::InvalidType}
										<< QDateTime{}
										<< QDateTime{};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("14.")
										  << false
										  << 14
										  << 3
										  << SubTerm::Type{SubTerm::Timepoint}
										  << QDateTime{QDate{cYear, cMonth, 10}, cTime}
										  << QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("14.")
										<< true
										<< 14
										<< 3
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, cMonth, 10}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("14.")
										<< false
										<< 14
										<< 3
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, cMonth, 20}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 14}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("14.")
									  << true
									  << 14
									  << 3
									  << SubTerm::Type{SubTerm::Timepoint}
									  << QDateTime{QDate{cYear, cMonth, 20}, cTime}
									  << QDateTime{QDate{cYear, cMonth + 1, 14}, cTime};
	QTest::addRow("offset.monthover.noKeep") << QStringLiteral("30.")
											 << false
											 << 30
											 << 3
											 << SubTerm::Type{SubTerm::Timepoint}
											 << QDateTime{QDate{cYear, 2, 28}, cTime}
											 << QDateTime{QDate{cYear, 2, 28}, cTime};
	QTest::addRow("offset.monthover.keep") << QStringLiteral("30.")
										   << true
										   << 30
										   << 3
										   << SubTerm::Type{SubTerm::Timepoint}
										   << QDateTime{QDate{cYear, 2, 28}, cTime}
										   << QDateTime{QDate{cYear, 3, 30}, cTime};

	QTest::addRow("loop.simple") << QStringLiteral("every 21st")
								 << false
								 << 21
								 << 10
								 << SubTerm::Type{SubTerm::LoopedTimePoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 21}, cTime};
	QTest::addRow("loop.long") << QStringLiteral("every 5. of")
							   << false
							   << 5
							   << 11
							   << SubTerm::Type{SubTerm::LoopedTimePoint}
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{cYear, cMonth, 5}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("on 4th of july")
								   << false
								   << 4
								   << 10
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 4}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every 3. in June")
								 << false
								 << 3
								 << 9
								 << SubTerm::Type{SubTerm::LoopedTimePoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 3}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in 10")
							 << false
							 << 0
							 << 0
							 << SubTerm::Type{SubTerm::InvalidScope}
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testMonthDayExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, day);
	QFETCH(int, offset);
	QFETCH(SubTerm::Type, type);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = MonthDayTerm::parse(expression.midRef(0)); //pass full str
	if(day > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, SubTerm::MonthDay);
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
	QTest::addColumn<SubTerm::Type>("type");
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
								<< SubTerm::Type{SubTerm::Timepoint}
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, cMonth, 18}, cTime};
	QTest::addRow("simple.short") << QStringLiteral("Fri")
								  << false
								  << 5
								  << 3
								  << SubTerm::Type{SubTerm::Timepoint}
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{cYear, cMonth, 19}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("Monday")
								 << false
								 << 1
								 << 6
								 << SubTerm::Type{SubTerm::Timepoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("on Wed")
								   << false
								   << 3
								   << 6
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 17}, cTime};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("Sat")
										  << false
										  << 6
										  << 3
										  << SubTerm::Type{SubTerm::Timepoint}
										  << QDateTime{QDate{cYear, cMonth, 17}, cTime}
										  << QDateTime{QDate{cYear, cMonth, 20}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("Sunday")
										<< true
										<< 7
										<< 6
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, cMonth, 17}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 21}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("Monday")
										<< false
										<< 1
										<< 6
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, cMonth, 17}, cTime}
										<< QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("Tue")
									  << true
									  << 2
									  << 3
									  << SubTerm::Type{SubTerm::Timepoint}
									  << QDateTime{QDate{cYear, cMonth, 17}, cTime}
									  << QDateTime{QDate{cYear, cMonth, 23}, cTime};
	QTest::addRow("offset.boundary.noKeep") << QStringLiteral("Wed")
											<< false
											<< 3
											<< 3
											<< SubTerm::Type{SubTerm::Timepoint}
											<< QDateTime{QDate{cYear, 12, 28}, cTime}
											<< QDateTime{QDate{cYear, 12, 26}, cTime};
	QTest::addRow("offset.boundary.keep") << QStringLiteral("Wed")
										  << true
										  << 3
										  << 3
										  << SubTerm::Type{SubTerm::Timepoint}
										  << QDateTime{QDate{cYear, 12, 28}, cTime}
										  << QDateTime{QDate{cYear + 1, 1, 2}, cTime};

	QTest::addRow("loop") << QStringLiteral("every Friday")
						  << false
						  << 5
						  << 12
						  << SubTerm::Type{SubTerm::LoopedTimePoint}
						  << QDateTime{cDate, cTime}
						  << QDateTime{QDate{cYear, cMonth, 19}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("on Monday next week")
								   << false
								   << 1
								   << 10
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, cMonth, 15}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every Wed in June")
								 << false
								 << 3
								 << 10
								 << SubTerm::Type{SubTerm::LoopedTimePoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 17}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("on Weddingday")
								 << false
								 << 3
								 << 6
								 << SubTerm::Type{SubTerm::Timepoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, cMonth, 17}, cTime};
	QTest::addRow("invalid") << QStringLiteral("on Thorsday")
							 << false
							 << 0
							 << 0
							 << SubTerm::Type{SubTerm::InvalidScope}
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testWeekDayExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, weekDay);
	QFETCH(int, offset);
	QFETCH(SubTerm::Type, type);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = WeekDayTerm::parse(expression.midRef(0)); //pass full str
	if(weekDay > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, SubTerm::WeekDay);
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
	QTest::addColumn<SubTerm::Type>("type");
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
								<< SubTerm::Type{SubTerm::Timepoint}
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, 5, 1}, cTime};
	QTest::addRow("simple.short") << QStringLiteral("Dec")
								  << false
								  << 12
								  << 3
								  << SubTerm::Type{SubTerm::Timepoint}
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{cYear, 12, 1}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("October")
								 << false
								 << 10
								 << 7
								 << SubTerm::Type{SubTerm::Timepoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 10, 1}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in June")
								   << false
								   << 6
								   << 7
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 6, 1}, cTime};

	QTest::addRow("offset.future.noKeep") << QStringLiteral("April")
										  << false
										  << 4
										  << 5
										  << SubTerm::Type{SubTerm::Timepoint}
										  << QDateTime{QDate{cYear, 2, 10}, cTime}
										  << QDateTime{QDate{cYear, 4, 1}, cTime};
	QTest::addRow("offset.future.keep") << QStringLiteral("July")
										<< true
										<< 7
										<< 4
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, 5, 10}, cTime}
										<< QDateTime{QDate{cYear, 7, 1}, cTime};
	QTest::addRow("offset.past.noKeep") << QStringLiteral("Feb")
										<< false
										<< 2
										<< 3
										<< SubTerm::Type{SubTerm::Timepoint}
										<< QDateTime{QDate{cYear, 7, 20}, cTime}
										<< QDateTime{QDate{cYear, 2, 1}, cTime};
	QTest::addRow("offset.past.keep") << QStringLiteral("March")
									  << true
									  << 3
									  << 5
									  << SubTerm::Type{SubTerm::Timepoint}
									  << QDateTime{QDate{cYear, 7, 20}, cTime}
									  << QDateTime{QDate{cYear + 1, 3, 1}, cTime};

	QTest::addRow("loop.short") << QStringLiteral("every Nov")
								<< false
								<< 11
								<< 9
								<< SubTerm::Type{SubTerm::LoopedTimePoint}
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{cYear, 11, 1}, cTime};
	QTest::addRow("loop.long") << QStringLiteral("every August")
							   << false
							   << 8
							   << 12
							   << SubTerm::Type{SubTerm::LoopedTimePoint}
							   << QDateTime{cDate, cTime}
							   << QDateTime{QDate{cYear, 8, 1}, cTime};

	QTest::addRow("substr.simple") << QStringLiteral("in September on 27th")
								   << false
								   << 9
								   << 13
								   << SubTerm::Type{SubTerm::Timepoint}
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{cYear, 9, 1}, cTime};
	QTest::addRow("substr.loop") << QStringLiteral("every September on 27th")
								 << false
								 << 9
								 << 16
								 << SubTerm::Type{SubTerm::LoopedTimePoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 9, 1}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("in Octobear")
								 << false
								 << 10
								 << 6
								 << SubTerm::Type{SubTerm::Timepoint}
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{cYear, 10, 1}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in Jarnurary")
							 << false
							 << 0
							 << 0
							 << SubTerm::Type{SubTerm::InvalidScope}
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testMonthExpressions()
{
	QFETCH(QString, expression);
	QFETCH(bool, applyRelative);
	QFETCH(int, month);
	QFETCH(int, offset);
	QFETCH(SubTerm::Type, type);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = MonthTerm::parse(expression.midRef(0)); //pass full str
	if(month > 0) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, SubTerm::Month);
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
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	// basic
	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("simple.short") << QStringLiteral("2015")
								  << 2015
								  << true
								  << 4
								  << QDateTime{cDate, cTime}
								  << QDateTime{QDate{2015, 1, 1}, cTime};
	QTest::addRow("simple.low") << QStringLiteral("0045")
								<< 45
								<< true
								<< 4
								<< QDateTime{cDate, cTime}
								<< QDateTime{QDate{45, 1, 1}, cTime};
	QTest::addRow("simple.negative") << QStringLiteral("-2015")
									 << -2015
									 << true
									 << 5
									 << QDateTime{cDate, cTime}
									 << QDateTime{QDate{-2015, 1, 1}, cTime};
	QTest::addRow("simple.long") << QStringLiteral("124563")
								 << 124563
								 << true
								 << 6
								 << QDateTime{cDate, cTime}
								 << QDateTime{QDate{124563, 1, 1}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in 0000")
								   << 0
								   << true
								   << 7
								   << QDateTime{cDate, cTime}
								   << QDateTime{QDate{0, 1, 1}, cTime};

	QTest::addRow("substr") << QStringLiteral("in 2017 in June")
							<< 2017
							<< true
							<< 8
							<< QDateTime{cDate, cTime}
							<< QDateTime{QDate{2017, 1, 1}, cTime};
	QTest::addRow("invalid") << QStringLiteral("in 95")
							 << 0
							 << false
							 << 0
							 << QDateTime{}
							 << QDateTime{};
}

void ParserTest::testYearExpressions()
{
	QFETCH(QString, expression);
	QFETCH(int, year);
	QFETCH(bool, valid);
	QFETCH(int, offset);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = YearTerm::parse(expression.midRef(0)); //pass full str
	if(valid) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, SubTerm::AbsoluteTimepoint);
		QCOMPARE(res.first->scope, SubTerm::Year);
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
	QTest::addColumn<SubTerm::Type>("type");
	QTest::addColumn<SubTerm::Scope>("scope");
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
									<< Seq{{SubTerm::Minute, 10}}
									<< 7
									<< SubTerm::Type{SubTerm::Timespan}
									<< SubTerm::Scope{SubTerm::Minute}
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour, cMin + 10}};
	QTest::addRow("simple.hours") << QStringLiteral("5 hours")
								  << Seq{{SubTerm::Hour, 5}}
								  << 7
								  << SubTerm::Type{SubTerm::Timespan}
								  << SubTerm::Scope{SubTerm::Hour}
								  << QDateTime{cDate, cTime}
								  << QDateTime{cDate, {cHour + 5, cMin}};
	QTest::addRow("simple.days") << QStringLiteral("1 day")
								 << Seq{{SubTerm::Day, 1}}
								 << 5
								 << SubTerm::Type{SubTerm::Timespan}
								 << SubTerm::Scope{SubTerm::Day}
								 << QDateTime{cDate, cTime}
								 << QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("simple.weeks") << QStringLiteral("2 weeks")
								  << Seq{{SubTerm::Week, 2}}
								  << 7
								  << SubTerm::Type{SubTerm::Timespan}
								  << SubTerm::Scope{SubTerm::Week}
								  << QDateTime{cDate, cTime}
								  << QDateTime{{cYear, cMonth, cDay + 14}, cTime};
	QTest::addRow("simple.months") << QStringLiteral("4 months")
								   << Seq{{SubTerm::Month, 4}}
								   << 8
								   << SubTerm::Type{SubTerm::Timespan}
								   << SubTerm::Scope{SubTerm::Month}
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth + 4, cDay}, cTime};
	QTest::addRow("simple.years") << QStringLiteral("40 years")
								   << Seq{{SubTerm::Year, 40}}
								   << 8
								   << SubTerm::Type{SubTerm::Timespan}
								   << SubTerm::Scope{SubTerm::Year}
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear + 40, cMonth, cDay}, cTime};
	QTest::addRow("simple.prefix") << QStringLiteral("in 3 days")
								   << Seq{{SubTerm::Day, 3}}
								   << 9
								   << SubTerm::Type{SubTerm::Timespan}
								   << SubTerm::Scope{SubTerm::Day}
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth, cDay + 3}, cTime};

	QTest::addRow("combined.times") << QStringLiteral("3 hours and 50 minutes")
									<< Seq{{SubTerm::Hour, 3}, {SubTerm::Minute, 50}}
									<< 22
									<< SubTerm::Type{SubTerm::Timespan}
									<< SubTerm::Scope{SubTerm::Minute | SubTerm::Hour}
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour + 4, 20}};
	QTest::addRow("combined.dates") << QStringLiteral("24 days and 5 years and 10 mons")
									<< Seq{{SubTerm::Day, 24}, {SubTerm::Year, 5}, {SubTerm::Month, 10}}
									<< 31
									<< SubTerm::Type{SubTerm::Timespan}
									<< SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::Day}
									<< QDateTime{cDate, cTime}
									<< QDateTime{{2024, 6, 5}, cTime};
	QTest::addRow("combined.all") << QStringLiteral("1 week and 2 mins and 3 years and 4 hours and 5 months and 6 days")
								  << Seq{{SubTerm::Week, 1}, {SubTerm::Minute, 2}, {SubTerm::Year, 3}, {SubTerm::Hour, 4}, {SubTerm::Month, 5}, {SubTerm::Day, 6}}
								  << 65
								  << SubTerm::Type{SubTerm::Timespan}
								  << SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::Week | SubTerm::Day | SubTerm::Hour | SubTerm::Minute}
								  << QDateTime{cDate, cTime}
								  << QDateTime{{cYear + 3, cMonth + 5, cDay + 13}, {cHour + 4, cMin + 2}};
	QTest::addRow("combined.prefix") << QStringLiteral("in 2 weeks and 30 mins")
									 << Seq{{SubTerm::Week, 2}, {SubTerm::Minute, 30}}
									 << 22
									 << SubTerm::Type{SubTerm::Timespan}
									 << SubTerm::Scope{SubTerm::Minute | SubTerm::Week}
									 << QDateTime{cDate, cTime}
									 << QDateTime{{cYear, cMonth, cDay + 14}, {cHour + 1, 00}};

	QTest::addRow("loop.simple") << QStringLiteral("every 3 weeks")
								 << Seq{{SubTerm::Week, 3}}
								 << 13
								 << SubTerm::Type{SubTerm::LoopedTimeSpan}
								 << SubTerm::Scope{SubTerm::Week}
								 << QDateTime{cDate, cTime}
								 << QDateTime{{cYear, cMonth + 1, 2}, cTime};
	QTest::addRow("loop.singular") << QStringLiteral("every day")
								   << Seq{{SubTerm::Day, 1}}
								   << 9
								   << SubTerm::Type{SubTerm::LoopedTimeSpan}
								   << SubTerm::Scope{SubTerm::Day}
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("loop.combined") << QStringLiteral("every 3 hours and 2 years and week")
								   << Seq{{SubTerm::Hour, 3}, {SubTerm::Year, 2}, {SubTerm::Week, 1}}
								   << 34
								   << SubTerm::Type{SubTerm::LoopedTimeSpan}
								   << SubTerm::Scope{SubTerm::Hour | SubTerm::Year | SubTerm::Week}
								   << QDateTime{cDate, cTime}
								   << QDateTime{{cYear + 2, cMonth, cDay + 7}, {cHour + 3, cMin}};

	QTest::addRow("substr.simple") << QStringLiteral("in 10 mins in 3 days")
									<< Seq{{SubTerm::Minute, 10}}
									<< 11
									<< SubTerm::Type{SubTerm::Timespan}
									<< SubTerm::Scope{SubTerm::Minute}
									<< QDateTime{cDate, cTime}
									<< QDateTime{cDate, {cHour, cMin + 10}};
	QTest::addRow("substr.loop") << QStringLiteral("every day of the year")
									<< Seq{{SubTerm::Day, 1}}
									<< 10
									<< SubTerm::Type{SubTerm::LoopedTimeSpan}
									<< SubTerm::Scope{SubTerm::Day}
									<< QDateTime{cDate, cTime}
									<< QDateTime{{cYear, cMonth, cDay + 1}, cTime};
	QTest::addRow("substr.half") << QStringLiteral("3 dayz")
									<< Seq{{SubTerm::Day, 3}}
									<< 5
									<< SubTerm::Type{SubTerm::Timespan}
									<< SubTerm::Scope{SubTerm::Day}
									<< QDateTime{cDate, cTime}
									<< QDateTime{{cYear, cMonth, cDay + 3}, cTime};

	QTest::addRow("invalid.format") << QStringLiteral("in 10 horas")
									<< Seq{}
									<< 0
									<< SubTerm::Type{SubTerm::InvalidType}
									<< SubTerm::Scope{SubTerm::InvalidScope}
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.number") << QStringLiteral("in day")
									<< Seq{}
									<< 0
									<< SubTerm::Type{SubTerm::InvalidType}
									<< SubTerm::Scope{SubTerm::InvalidScope}
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.combined") << QStringLiteral("in 2 mins and 5 minutes")
									  << Seq{}
									  << 0
									  << SubTerm::Type{SubTerm::InvalidType}
									  << SubTerm::Scope{SubTerm::InvalidScope}
									  << QDateTime{}
									  << QDateTime{};
	QTest::addRow("invalid.substr") << QStringLiteral("in 10 mins and in 3 days")
									<< Seq{}
									<< 0
									<< SubTerm::Type{SubTerm::InvalidType}
									<< SubTerm::Scope{SubTerm::InvalidScope}
									<< QDateTime{}
									<< QDateTime{};
	QTest::addRow("invalid.loop") << QStringLiteral("every 10 horas")
								  << Seq{}
								  << 0
								  << SubTerm::Type{SubTerm::InvalidType}
								  << SubTerm::Scope{SubTerm::InvalidScope}
								  << QDateTime{}
								  << QDateTime{};
}

void ParserTest::testSequenceExpressions()
{
	QFETCH(QString, expression);
	QFETCH(SequenceTerm::Sequence, sequence);
	QFETCH(int, offset);
	QFETCH(SubTerm::Type, type);
	QFETCH(SubTerm::Scope, scope);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	// first: parse and verify parse result
	auto res = SequenceTerm::parse(expression.midRef(0)); //pass full str
	if(!sequence.isEmpty()) {
		QVERIFY(res.first);
		QCOMPARE(res.first->type, type);
		QCOMPARE(res.first->scope, scope);
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
		QCOMPARE(res.first->type, SubTerm::Timespan);
		QCOMPARE(res.first->scope, SubTerm::Day);
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
	QTest::addColumn<SubTerm::Scope>("scope");
	QTest::addColumn<bool>("looped");
	QTest::addColumn<bool>("absolute");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("time") << QStringLiteral("14:00")
						  << 1
						  << SubTerm::Scope{SubTerm::Hour | SubTerm::Minute}
						  << false
						  << false
						  << QDateTime{cDate, {10, 0}}
						  << QDateTime{cDate, {14, 0}};
	QTest::addRow("date") << QStringLiteral("March 24th")
						  << 2
						  << SubTerm::Scope{SubTerm::MonthDay | SubTerm::Month}
						  << false
						  << false
						  << QDateTime{{2018, 2, 10}, cTime}
						  << QDateTime{{2018, 3, 24}, cTime};
	QTest::addRow("datetime") << QStringLiteral("on March the 24th at 14:00")
							  << 3
							  << SubTerm::Scope{SubTerm::MonthDay | SubTerm::Month | SubTerm::Hour | SubTerm::Minute}
							  << false
							  << false
							  << QDateTime{{2018, 2, 10}, cTime}
							  << QDateTime{{2018, 3, 24}, {14, 0}};

	QTest::addRow("parts.date") << QStringLiteral("every 2 years and 3 months on Saturday")
								<< 2
								<< SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::WeekDay}
								<< true
								<< false
								<< QDateTime{{2018, 2, 10}, cTime}
								<< QDateTime{{2020, 5, 9}, cTime};
	QTest::addRow("parts.datetime") << QStringLiteral("in 2019 on 24.10. at quarter past 10")
									<< 3
									<< SubTerm::Scope{SubTerm::Year | SubTerm::Month | SubTerm::MonthDay | SubTerm::Hour | SubTerm::Minute}
									<< false
									<< true
									<< QDateTime{{2010, 11, 11}, cTime}
									<< QDateTime{{2019, 10, 24}, {10, 15}};
	QTest::addRow("loop.subscope") << QStringLiteral("every week In November")
								   << 2
								   << SubTerm::Scope{SubTerm::Month | SubTerm::Week}
								   << true
								   << false
								   << QDateTime{{2018, 7, 15}, cTime}
								   << QDateTime{{2018, 11, 1}, cTime};

	QTest::addRow("invalid.scope") << QStringLiteral("in April at 4 o'clock on 24.12.")
								   << 0
								   << SubTerm::Scope{SubTerm::InvalidScope}
								   << false
								   << false
								   << QDateTime{}
								   << QDateTime{};
	QTest::addRow("invalid.loop.double") << QStringLiteral("every 3 days every 20 minutes")
										 << 0
										 << SubTerm::Scope{SubTerm::InvalidScope}
										 << false
										 << false
										 << QDateTime{}
										 << QDateTime{};
	QTest::addRow("invalid.loop.span") << QStringLiteral("every 3 days in 20 minutes")
									   << 0
									   << SubTerm::Scope{SubTerm::InvalidScope}
									   << false
									   << false
									   << QDateTime{}
									   << QDateTime{};
	QTest::addRow("invalid.pointspan") << QStringLiteral("In November in 3 weeks")
									   << 0
									   << SubTerm::Scope{SubTerm::InvalidScope}
									   << false
									   << false
									   << QDateTime{}
									   << QDateTime{};
	QTest::addRow("invalid.span.double") << QStringLiteral("in 3 hours 20 mins")
										 << 0
										 << SubTerm::Scope{SubTerm::InvalidScope}
										 << false
										 << false
										 << QDateTime{}
										 << QDateTime{};
}

void ParserTest::testExpressionParsing()
{
	QFETCH(QString, expression);
	QFETCH(int, depth);
	QFETCH(SubTerm::Scope, scope);
	QFETCH(bool, looped);
	QFETCH(bool, absolute);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	auto terms = parser->parseExpression(expression);
	if(depth == 0)
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
	else {
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
										{cDate, {10, 00}},
										{cDate, cTime},
										{{2018, 12, 13}, cTime},
										{cDate, {10, 00}}
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
	QTest::addRow("limit.from") << QStringLiteral("every day from 10. of June")
								<< true
								<< QDateTime{{2017, 10, 16}, cTime}
								<< QDateTime{{2018, 6, 10}, cTime}
								<< QDateTime{}
								<< QDateTime{};
	QTest::addRow("limit.until") << QStringLiteral("every Tuesday until 2020")
								 << true
								 << QDateTime{}
								 << QDateTime{}
								 << QDateTime{{2018, 1, 2}, cTime}
								 << QDateTime{{2020, 1, 1}, cTime};
	//	QTest::addRow("limit.from-until") << QStringLiteral("every 20 minutes from 10 to 14")
	//									 << true
	//									 << QDateTime{{2017, 10, 16}, cTime}
	//									 << QDateTime{{2018, 6, 10}, cTime}
	//									 << QDateTime{}
	//									 << QDateTime{}; //TODO use to test for duplicates
	QTest::addRow("limit.from-until") << QStringLiteral("every 20 minutes from 10:00 to 17:15")
									  << true
									  << QDateTime{cDate, {8, 0}}
									  << QDateTime{cDate, {10, 0}}
									  << QDateTime{cDate, {8, 0}}
									  << QDateTime{cDate, {17, 15}};
	QTest::addRow("limit.until-from") << QStringLiteral("every year until June from 2015")
									  << true
									  << QDateTime{{2010, 7, 15}, cTime}
									  << QDateTime{{2015, 1, 1}, cTime}
									  << QDateTime{{2018, 4, 5}, cTime}
									  << QDateTime{{2018, 6, 1}, cTime};

	QTest::addRow("fence.from") << QStringLiteral("every day in April from 2020")
								<< true
								<< QDateTime{{2017, 10, 16}, cTime}
								<< QDateTime{{2020, 1, 1}, cTime}
								<< QDateTime{}
								<< QDateTime{};
	QTest::addRow("fence.until") << QStringLiteral("every minute on 24th until in 1 month")
								 << true
								 << QDateTime{}
								 << QDateTime{}
								 << QDateTime{{2018, 1, 2}, cTime}
								 << QDateTime{{2018, 2, 2}, cTime};
	QTest::addRow("fence.from-until") << QStringLiteral("every 20 minutes on Tuesday from 2010 to 2020")
									  << true
									  << QDateTime{{2000, 1, 2}, cTime}
									  << QDateTime{{2010, 1, 1}, cTime}
									  << QDateTime{{2000, 1, 2}, cTime}
									  << QDateTime{{2020, 1, 1}, cTime};



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
	QTest::addRow("invalid.subloop.from") << QStringLiteral("every day from every month")
										  << false
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{}
										  << QDateTime{};
	QTest::addRow("invalid.subloop.until") << QStringLiteral("every Tuesday until every Month")
										   << false
										   << QDateTime{}
										   << QDateTime{}
										   << QDateTime{}
										   << QDateTime{};
	QTest::addRow("invalid.fence.overlap") << QStringLiteral("on the 24th every minute until in 10 days")
										   << false
										   << QDateTime{}
										   << QDateTime{}
										   << QDateTime{}
										   << QDateTime{};
	QTest::addRow("invalid.fence.inverted") << QStringLiteral("on the 24th every minute until 10:30")
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
			if(subTerm->type == SubTerm::FromSubterm) {
				QVERIFY2(sinceFrom.isValid(), "Found unexpected from term");
				QVERIFY(subTerm.dynamicCast<LimiterTerm>());
				auto res = subTerm.staticCast<LimiterTerm>()->limitTerm().apply(sinceFrom);
				QCOMPARE(res, resultFrom);
				sinceFrom = QDateTime{};
			}
			if(subTerm->type == SubTerm::UntilSubTerm) {
				QVERIFY2(sinceUntil.isValid(), "Found unexpected until term");
				QVERIFY(subTerm.dynamicCast<LimiterTerm>());
				auto res = subTerm.staticCast<LimiterTerm>()->limitTerm().apply(sinceUntil);
				QCOMPARE(res, resultUntil);
				sinceUntil = QDateTime{};
			}
		}

		QVERIFY2(!sinceFrom.isValid(), "Expected from term but none was found");
		QVERIFY2(!sinceUntil.isValid(), "Expected until term but none was found");
	} else
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
}

void ParserTest::testTermSplitting_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<int>("loopSize");
	QTest::addColumn<int>("fenceSize");
	QTest::addColumn<int>("fromSize");
	QTest::addColumn<int>("untilSize");

	QTest::addRow("single.pureloop") << QStringLiteral("every June the 22nd")
									 << 2
									 << 0
									 << 0
									 << 0;
	QTest::addRow("single.fenced") << QStringLiteral("every 22nd of June")
								   << 1
								   << 1
								   << 0
								   << 0;
	QTest::addRow("single.from") << QStringLiteral("every 3 days from tomorrow")
								 << 1
								 << 0
								 << 1
								 << 0;
	QTest::addRow("single.until") << QStringLiteral("every 2 hours and 20 mins until 22:00")
								  << 1
								  << 0
								  << 0
								  << 1;
	QTest::addRow("multi.longfence") << QStringLiteral("every day at 22:00 in 2020 in December")
									 << 2
									 << 2
									 << 0
									 << 0;
	QTest::addRow("multi.from-until") << QStringLiteral("every 3 days from tomorrow until 25th")
									  << 1
									  << 0
									  << 1
									  << 1;
	QTest::addRow("multi.all") << QStringLiteral("every day at 22:00 in December from 2020 until 2025")
							   << 2
							   << 1
							   << 1
							   << 1;
	QTest::addRow("invalid.noloop") << QStringLiteral("in 10 days")
									<< 0
									<< 0
									<< 0
									<< 0;
}

void ParserTest::testTermSplitting()
{
	QFETCH(QString, expression);
	QFETCH(int, loopSize);
	QFETCH(int, fenceSize);
	QFETCH(int, fromSize);
	QFETCH(int, untilSize);

	auto terms = parser->parseExpression(expression);
	QCOMPARE(terms.size(), 1);
	Term loop, fence, from, until;
	std::tie(loop, fence, from, until) = terms.first().splitLoop();
	QCOMPARE(loop.size(), loopSize);
	QCOMPARE(fence.size(), fenceSize);
	QCOMPARE(from.size(), fromSize);
	QCOMPARE(until.size(), untilSize);
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
	QTest::addRow("invalid.loop") << QStringLiteral("every Wed at half past 3 am")
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

void ParserTest::testSingularSchedules_data()
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

void ParserTest::testSingularSchedules()
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
		auto res = parser->createSchedule(terms.first(), since);
		if(result.isValid()) {
			QVERIFY(res);
			QVERIFY(!res->isRepeating());
			QCOMPARE(res->current(), result);
			QVERIFY(!res->nextSchedule().isValid());
		} else
			QVERIFY(!res);
	}
}

void ParserTest::testRepeatedSchedules_data()
{
	QTest::addColumn<QString>("expression");
	QTest::addColumn<QTime>("parserTime");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QList<QDateTime>>("results");

	const auto cDate = QDate::currentDate();
	const auto cTime = QTime::currentTime();
	QTest::addRow("simple.span") << QStringLiteral("every day")
								 << QTime{9, 0}
								 << QDateTime{cDate, cTime}
								 << QList<QDateTime>{
										  {cDate.addDays(1), {9, 0}},
										  {cDate.addDays(2), {9, 0}},
										  {cDate.addDays(3), {9, 0}},
										  {cDate.addDays(4), {9, 0}},
									  };
	QTest::addRow("simple.point") << QStringLiteral("every March the 21st")
								  << QTime{9, 0}
								  << QDateTime{{2018, 2, 3}, cTime}
								  << QList<QDateTime>{
										   {{2018, 3, 21}, {9, 0}},
										   {{2019, 3, 21}, {9, 0}},
										   {{2020, 3, 21}, {9, 0}},
										   {{2021, 3, 21}, {9, 0}},
									   };
	QTest::addRow("simple.mixed") << QStringLiteral("every 2 months and 3 days at 17:30")
								  << QTime{9, 0}
								  << QDateTime{{2018, 7, 3}, cTime}
								  << QList<QDateTime>{
										   {{2018, 9, 6}, {17, 30}},
										   {{2018, 11, 9}, {17, 30}},
										   {{2019, 1, 12}, {17, 30}},
										   {{2019, 3, 15}, {17, 30}},
									   };

	QTest::addRow("fenced.singular") << QStringLiteral("every day in October")
									 << QTime{10, 0}
									 << QDateTime{{2018, 7, 3}, cTime}
									 << QList<QDateTime>{
											  {{2018, 10, 1}, {10, 0}},
											  {{2018, 10, 2}, {10, 0}},
											  {{2018, 10, 3}, {10, 0}},
											  {{2018, 10, 4}, {10, 0}},
										  };
	QTest::addRow("fenced.gaped") << QStringLiteral("every 2 Weeks in November")
								  << QTime{9, 0}
								  << QDateTime{{2018, 12, 3}, cTime}
								  << QList<QDateTime>{
										   {{2019, 11, 8}, {9, 0}},
										   {{2019, 11, 22}, {9, 0}},
										   {{2020, 11, 8}, {9, 0}},
										   {{2020, 11, 22}, {9, 0}},
									   };
	QTest::addRow("fenced.elaborate") << QStringLiteral("every 2 Weeks on Saturday at quarter past 3 pm in November")
									  << QTime{9, 0}
									  << QDateTime{{2018, 2, 3}, cTime}
									  << QList<QDateTime>{
											   {{2018, 11, 10}, {15, 15}},
											   {{2018, 11, 24}, {15, 15}},
											   {{2019, 11, 9}, {15, 15}},
											   {{2019, 11, 23}, {15, 15}},
										   };

	QTest::addRow("limits.from") << QStringLiteral("every 20 minutes from 14:30")
								 << QTime{9, 0}
								 << QDateTime{cDate, {17, 00}}
								 << QList<QDateTime>{
										  {cDate.addDays(1), {14, 50}},
										  {cDate.addDays(1), {15, 10}},
										  {cDate.addDays(1), {15, 30}},
										  {cDate.addDays(1), {15, 50}},
										  {cDate.addDays(1), {16, 10}},
									  };
	QTest::addRow("limits.until") << QStringLiteral("every 3 Months on 27th until 2019")
								  << QTime{9, 0}
								  << QDateTime{{2018, 2, 11}, cTime}
								  << QList<QDateTime>{
										   {{2018, 5, 27}, {9, 0}},
										   {{2018, 8, 27}, {9, 0}},
										   {{2018, 11, 27}, {9, 0}},
										   {},
									   };
	QTest::addRow("limits.both") << QStringLiteral("every Tuesday at 10 o'clock from the 27th until the 12th")
								 << QTime{9, 0}
								 << QDateTime{{2018, 8, 24}, cTime}
								 << QList<QDateTime>{
										  {{2018, 8, 28}, {10, 0}},
										  {{2018, 9, 4}, {10, 0}},
										  {{2018, 9, 11}, {10, 0}},
										  {},
									  };
	QTest::addRow("limits.close") << QStringLiteral("every Tuesday at 10 o'clock from the 28th to the 11th")
								  << QTime{9, 0}
								  << QDateTime{{2018, 8, 24}, cTime}
								  << QList<QDateTime>{
										   {{2018, 9, 4}, {10, 0}},
										   {{2018, 9, 11}, {10, 0}},
										   {},
									   };

	QTest::addRow("combined.from") << QStringLiteral("every Monday in April from 2025")
								   << QTime{9, 0}
								   << QDateTime{{2018, 8, 24}, cTime}
								   << QList<QDateTime>{
											{{2025, 4, 7}, {9, 0}},
											{{2025, 4, 14}, {9, 0}},
											{{2025, 4, 21}, {9, 0}},
											{{2025, 4, 28}, {9, 0}},
											{{2026, 4, 6}, {9, 0}},
										};
	QTest::addRow("combined.until") << QStringLiteral("every 7 hours and 20 minutes on Monday until in 2 weeks")
									<< QTime{9, 0}
									<< QDateTime{{2018, 8, 24}, cTime}
									<< QList<QDateTime>{
											 {{2018, 8, 27}, {7, 20}},
											 {{2018, 8, 27}, {14, 40}},
											 {{2018, 8, 27}, {22, 0}},
											 {{2018, 9, 3}, {7, 20}},
											 {{2018, 9, 3}, {14, 40}},
											 {{2018, 9, 3}, {22, 0}},
											 {},
										 };
	QTest::addRow("combined.all") << QStringLiteral("every 7 hours and 20 minutes on Monday until in 2 weeks from in 1 week")
								  << QTime{9, 0}
								  << QDateTime{{2018, 8, 24}, cTime}
								  << QList<QDateTime>{
										   {{2018, 9, 3}, {7, 20}},
										   {{2018, 9, 3}, {14, 40}},
										   {{2018, 9, 3}, {22, 0}},
										   {},
									   };

	QTest::addRow("invalid.inverse") << QStringLiteral("every 10 minutes from in 1 month until tomorrow")
									 << QTime{9, 0}
									 << QDateTime{{2018, 8, 24}, cTime}
									 << QList<QDateTime>{};
}

void ParserTest::testRepeatedSchedules()
{
	QFETCH(QString, expression);
	QFETCH(QTime, parserTime);
	QFETCH(QDateTime, since);
	QFETCH(QList<QDateTime>, results);

	parser->_settings->scheduler.defaultTime = parserTime;
	auto terms = parser->parseExpression(expression);
	if(!since.isValid())
		QVERIFY(terms.isEmpty()); //TODO verify the correct error message
	else {
		QCOMPARE(terms.size(), 1);
		auto res = parser->createSchedule(terms.first(), since);
		if(!results.isEmpty()) {
			QVERIFY(res);
			QVERIFY(res->isRepeating());

			auto isFirst = true;
			for(const auto &result : results) {
				if(isFirst)
					isFirst = false;
				else
					QCOMPARE(res->nextSchedule(), result);
				QCOMPARE(res->current(), result);
			}
		} else
			QVERIFY(!res);
	}
}

QTEST_MAIN(ParserTest)

#include "tst_parser.moc"
