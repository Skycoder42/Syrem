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
	void testOneTimeReminder_data();
	void testOneTimeReminder();

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

void CoreReminderTest::testOneTimeReminder_data()
{
	QTest::addColumn<QString>("query");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	QTest::newRow("<empty>") << QString()
							 << QDateTime()
							 << QDateTime();
}

void CoreReminderTest::testOneTimeReminder()
{
	QFETCH(QString, query);
	QFETCH(QDateTime, since);
	QFETCH(QDateTime, result);

	auto expr = parser->parse(query);
	if(result.isValid()) {
		QVERIFY(expr);
		auto sched = expr->createSchedule(since, this);
		QVERIFY(sched);
		QVERIFY(!sched->isRepeating());
		QCOMPARE(sched->nextSchedule(since), result);
		sched->deleteLater();
	} else
		QVERIFY(!expr);
}

QTEST_MAIN(CoreReminderTest)

#include "tst_coreremindertest.moc"
