#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <dateparser.h>

class CoreReminderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testTimePointReminder_data();
	void testTimePointReminder();

	//TODO test others with 3 times, create, apply, result
private:
	DateParser *parser;
};

void CoreReminderTest::initTestCase()
{
	parser = new DateParser(this);
}

void CoreReminderTest::cleanupTestCase()
{
	parser->deleteLater();
}

void CoreReminderTest::testTimePointReminder_data()
{
	QTest::addColumn<QString>("query");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	QTest::newRow("<empty>") << QString()
							 << QDateTime()
							 << QDateTime();

	//timepoint.date
	QTest::newRow("date") << QStringLiteral("24-11-2017")
						  << QDateTime({2017, 11, 1})
						  << QDateTime({2017, 11, 24});
	QTest::newRow("date.same") << QStringLiteral("24-11-2017")
							   << QDateTime({2017, 11, 24}, {17, 30})
							   << QDateTime();
	QTest::newRow("date.after") << QStringLiteral("24-11-2017")
								<< QDateTime({2017, 12, 1})
								<< QDateTime();
	QTest::newRow("date.prefix.1") << QStringLiteral("on 24-11-2017")
								   << QDateTime({2017, 11, 1})
								   << QDateTime({2017, 11, 24});
	QTest::newRow("date.prefix.2") << QStringLiteral("next 24-11-2017")
								   << QDateTime({2017, 11, 1})
								   << QDateTime({2017, 11, 24});
	QTest::newRow("date.prefix.invalid") << QStringLiteral("nextes 24-11-2017")
										 << QDateTime()
										 << QDateTime();
	QTest::newRow("date.time") << QStringLiteral("24-11-2017 15:30")
							   << QDateTime({2017, 11, 1}, {15, 00})
							   << QDateTime({2017, 11, 24}, {15, 30});
	QTest::newRow("date.time.same") << QStringLiteral("24-11-2017 15:30")
									<< QDateTime({2017, 11, 24}, {15, 30})
									<< QDateTime();
	QTest::newRow("date.time.after") << QStringLiteral("24-11-2017 15:30")
									 << QDateTime({2017, 11, 24}, {16, 00})
									 << QDateTime();
	QTest::newRow("date.time.prefix") << QStringLiteral("on 24-11-2017 at 15:30")
									  << QDateTime({2017, 11, 1}, {15, 00})
									  << QDateTime({2017, 11, 24}, {15, 30});

	//timepoint.datum.weekday
	//timepoint.datum.day
	//timepoint.datum.month
	//timepoint.datum.monthday
	//timepoint.year
	//timepoint.ahead
}

void CoreReminderTest::testTimePointReminder()
{
	QFETCH(QString, query);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	auto expr = parser->parse(query);
	if(since.isValid()) {
		QVERIFY(expr);
		auto sched = expr->createSchedule(since, this);
		if(result.isValid()) {
			QVERIFY(sched);
			QVERIFY(!sched->isRepeating());
			QCOMPARE(sched->nextSchedule(since), result);
			sched->deleteLater();
		} else
			QVERIFY(!sched);
		expr->deleteLater();
	} else
		QVERIFY(!expr);
}

QTEST_MAIN(CoreReminderTest)

#include "tst_coreremindertest.moc"
