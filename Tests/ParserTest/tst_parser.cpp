#include <QtTest>
#define private public
#include <eventexpressionparser.h>
#undef private
using namespace Expressions;

class ParserTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	// Test to regex conversion
	void testTimeRegexConversion_data();
	void testTimeRegexConversion();
	void testTimeExpressions_data();
	void testTimeExpressions();

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

QTEST_MAIN(ParserTest)

#include "tst_parser.moc"
