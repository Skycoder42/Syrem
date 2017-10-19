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

	void testTimeSpanReminder_data();
	void testTimeSpanReminder();

	//TODO test others with 3 times, create, apply, result
private:
	DateParser *parser;
};

void CoreReminderTest::initTestCase()
{
	QLocale::setDefault(QLocale::c());
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
	QTest::newRow("date") << QStringLiteral("24-10-2017")
						  << QDateTime({2017, 10, 1})
						  << QDateTime({2017, 10, 24});
	QTest::newRow("date.same") << QStringLiteral("24-10-2017")
							   << QDateTime({2017, 10, 24}, {17, 30})
							   << QDateTime();
	QTest::newRow("date.after") << QStringLiteral("24-10-2017")
								<< QDateTime({2017, 12, 1})
								<< QDateTime();
	QTest::newRow("date.prefix.1") << QStringLiteral("on 24-10-2017")
								   << QDateTime({2017, 10, 1})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("date.prefix.2") << QStringLiteral("next 24-10-2017")
								   << QDateTime({2017, 10, 1})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("date.prefix.invalid") << QStringLiteral("nextes 24-10-2017")
										 << QDateTime()
										 << QDateTime();
	QTest::newRow("date.time") << QStringLiteral("24-10-2017 15:30")
							   << QDateTime({2017, 10, 24}, {15, 00})
							   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("date.time.same") << QStringLiteral("24-10-2017 15:30")
									<< QDateTime({2017, 10, 24}, {15, 30})
									<< QDateTime();
	QTest::newRow("date.time.after") << QStringLiteral("24-10-2017 15:30")
									 << QDateTime({2017, 10, 24}, {16, 00})
									 << QDateTime();
	QTest::newRow("date.time.prefix") << QStringLiteral("on 24-10-2017 at 15:30")
									  << QDateTime({2017, 10, 24}, {15, 00})
									  << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.datum.weekday
	QTest::newRow("weekday") << QStringLiteral("tuesday")
							 << QDateTime({2017, 10, 23})
							 << QDateTime({2017, 10, 24});
	QTest::newRow("weekday.same") << QStringLiteral("tuesday")
								  << QDateTime({2017, 10, 17})
								  << QDateTime({2017, 10, 24});
	QTest::newRow("weekday.after") << QStringLiteral("tuesday")
								   << QDateTime({2017, 10, 19})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("weekday.prefix.1") << QStringLiteral("on tuesday")
									  << QDateTime({2017, 10, 19})
									  << QDateTime({2017, 10, 24});
	QTest::newRow("weekday.prefix.2") << QStringLiteral("next tuesday")
									  << QDateTime({2017, 10, 19})
									  << QDateTime({2017, 10, 24});
	QTest::newRow("weekday.time") << QStringLiteral("tuesday 15:30")
								  << QDateTime({2017, 10, 19}, {15, 00})
								  << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("weekday.time.same") << QStringLiteral("tuesday 15:30")
									   << QDateTime({2017, 10, 17}, {15, 30})
									   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("weekday.time.after") << QStringLiteral("tuesday 15:30")
										<< QDateTime({2017, 10, 17}, {16, 00})
										<< QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("weekday.time.prefix") << QStringLiteral("on tuesday at 15:30")
										 << QDateTime({2017, 10, 17}, {15, 00})
										 << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.datum.day
	QTest::newRow("day") << QStringLiteral("24.")
						 << QDateTime({2017, 10, 10})
						 << QDateTime({2017, 10, 24});
	QTest::newRow("day.same") << QStringLiteral("24.")
							  << QDateTime({2017, 10, 24})
							  << QDateTime({2017, 11, 24});
	QTest::newRow("day.after") << QStringLiteral("24.")
							   << QDateTime({2017, 10, 30})
							   << QDateTime({2017, 11, 24});
	QTest::newRow("day.prefix.1") << QStringLiteral("on 24.")
								  << QDateTime({2017, 10, 10})
								  << QDateTime({2017, 10, 24});
	QTest::newRow("day.prefix.2") << QStringLiteral("next 24.")
								  << QDateTime({2017, 10, 10})
								  << QDateTime({2017, 10, 24});
	QTest::newRow("day.time") << QStringLiteral("24. 15:30")
							  << QDateTime({2017, 10, 19}, {15, 00})
							  << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("day.time.same") << QStringLiteral("24. 15:30")
								   << QDateTime({2017, 10, 24}, {15, 30})
								   << QDateTime({2017, 11, 24}, {15, 30});
	QTest::newRow("day.time.after") << QStringLiteral("24. 15:30")
									<< QDateTime({2017, 10, 24}, {16, 00})
									<< QDateTime({2017, 11, 24}, {15, 30});
	QTest::newRow("day.time.prefix") << QStringLiteral("on 24. at 15:30")
									 << QDateTime({2017, 10, 19}, {15, 00})
									 << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.datum.month
	QTest::newRow("month") << QStringLiteral("October")
						   << QDateTime({2017, 9, 24})
						   << QDateTime({2017, 10, 24});
	QTest::newRow("month.same") << QStringLiteral("October")
								<< QDateTime({2017, 10, 24})
								<< QDateTime({2018, 10, 24});
	QTest::newRow("month.after") << QStringLiteral("October")
								 << QDateTime({2017, 11, 30})
								 << QDateTime({2018, 10, 30});
	QTest::newRow("month.prefix.1") << QStringLiteral("on October")
									<< QDateTime({2017, 9, 24})
									<< QDateTime({2017, 10, 24});
	QTest::newRow("month.prefix.2") << QStringLiteral("next October")
									<< QDateTime({2017, 9, 24})
									<< QDateTime({2017, 10, 24});
	QTest::newRow("month.time") << QStringLiteral("October 15:30")
								<< QDateTime({2017, 9, 24}, {15, 00})
								<< QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("month.time.same") << QStringLiteral("October 15:30")
									 << QDateTime({2017, 10, 24}, {15, 30})
									 << QDateTime({2018, 10, 24}, {15, 30});
	QTest::newRow("month.time.after") << QStringLiteral("October 15:30")
									  << QDateTime({2017, 11, 30}, {16, 00})
									  << QDateTime({2018, 10, 30}, {15, 30});
	QTest::newRow("month.time.prefix") << QStringLiteral("on October at 15:30")
									   << QDateTime({2017, 9, 24}, {15, 00})
									   << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.datum.monthday
	QTest::newRow("monthday") << QStringLiteral("24.10.")
							  << QDateTime({2017, 9, 10})
							  << QDateTime({2017, 10, 24});
	QTest::newRow("monthday.named") << QStringLiteral("24. October")
									<< QDateTime({2017, 9, 10})
									<< QDateTime({2017, 10, 24});
	QTest::newRow("monthday.same") << QStringLiteral("24.10.")
								   << QDateTime({2017, 10, 24})
								   << QDateTime({2018, 10, 24});
	QTest::newRow("monthday.after") << QStringLiteral("24.10.")
									<< QDateTime({2017, 10, 25})
									<< QDateTime({2018, 10, 24});
	QTest::newRow("monthday.prefix.1") << QStringLiteral("on 24.10.")
									   << QDateTime({2017, 9, 10})
									   << QDateTime({2017, 10, 24});
	QTest::newRow("monthday.prefix.2") << QStringLiteral("next 24. October")
									   << QDateTime({2017, 9, 10})
									   << QDateTime({2017, 10, 24});
	QTest::newRow("monthday.time") << QStringLiteral("24.10. 15:30")
								   << QDateTime({2017, 9, 10}, {15, 00})
								   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("monthday.time.same") << QStringLiteral("24.10. 15:30")
										<< QDateTime({2017, 10, 24}, {15, 30})
										<< QDateTime({2018, 10, 24}, {15, 30});
	QTest::newRow("monthday.time.after") << QStringLiteral("24.10. 15:30")
										 << QDateTime({2017, 10, 24}, {16, 00})
										 << QDateTime({2018, 10, 24}, {15, 30});
	QTest::newRow("monthday.time.prefix") << QStringLiteral("on 24. October at 15:30")
										  << QDateTime({2017, 10, 19}, {15, 00})
										  << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.year
	QTest::newRow("year") << QStringLiteral("2017")
						  << QDateTime({2015, 10, 24})
						  << QDateTime({2017, 10, 24});
	QTest::newRow("year.same") << QStringLiteral("2017")
							   << QDateTime({2017, 10, 24})
							   << QDateTime();
	QTest::newRow("year.after") << QStringLiteral("2017")
								<< QDateTime({2018, 11, 30})
								<< QDateTime();
	QTest::newRow("year.prefix.1") << QStringLiteral("on 2017")
								   << QDateTime({2015, 10, 24})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("year.prefix.2") << QStringLiteral("next 2017")
								   << QDateTime({2015, 10, 24})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("year.time") << QStringLiteral("2017 15:30")
							   << QDateTime({2015, 10, 24}, {15, 00})
							   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("year.time.same") << QStringLiteral("2017 15:30")
									<< QDateTime({2017, 10, 24}, {15, 30})
									<< QDateTime();
	QTest::newRow("year.time.after") << QStringLiteral("2017 15:30")
									 << QDateTime({2017, 11, 30}, {16, 00})
									 << QDateTime();
	QTest::newRow("year.time.prefix") << QStringLiteral("on 2017 at 15:30")
									  << QDateTime({2015, 10, 24}, {15, 00})
									  << QDateTime({2017, 10, 24}, {15, 30});

	//timepoint.ahead.today
	auto today = QDate::currentDate();
	QTest::newRow("ahead.today") << QStringLiteral("today")
								 << QDateTime(today)
								 << QDateTime();
	QTest::newRow("ahead.today.prefix.1") << QStringLiteral("on today")
										  << QDateTime(today)
										  << QDateTime();
	QTest::newRow("ahead.today.prefix.2") << QStringLiteral("next today")
										  << QDateTime(today)
										  << QDateTime();
	QTest::newRow("ahead.today.time") << QStringLiteral("today 15:30")
									  << QDateTime(today, {15, 00})
									  << QDateTime(today, {15, 30});
	QTest::newRow("ahead.today.time.same") << QStringLiteral("today 15:30")
										   << QDateTime(today, {15, 30})
										   << QDateTime();
	QTest::newRow("ahead.today.time.after") << QStringLiteral("today 15:30")
											<< QDateTime(today, {16, 00})
											<< QDateTime();
	QTest::newRow("ahead.today.time.prefix") << QStringLiteral("on today at 15:30")
											 << QDateTime(today, {15, 00})
											 << QDateTime(today, {15, 30});

	//timepoint.ahead.tomorrow
	auto tomorrow = today.addDays(1);
	QTest::newRow("ahead.tomorrow") << QStringLiteral("tomorrow")
									<< QDateTime(today)
									<< QDateTime(tomorrow);
	QTest::newRow("ahead.tomorrow.prefix.1") << QStringLiteral("on tomorrow")
											 << QDateTime(today)
											 << QDateTime(tomorrow);
	QTest::newRow("ahead.tomorrow.prefix.2") << QStringLiteral("next tomorrow")
											 << QDateTime(today)
											 << QDateTime(tomorrow);
	QTest::newRow("ahead.tomorrow.time") << QStringLiteral("tomorrow 15:30")
										 << QDateTime(today, {15, 00})
										 << QDateTime(tomorrow, {15, 30});
	QTest::newRow("ahead.tomorrow.time.same") << QStringLiteral("tomorrow 15:30")
											  << QDateTime(tomorrow, {15, 30})
											  << QDateTime();
	QTest::newRow("ahead.tomorrow.time.after") << QStringLiteral("tomorrow 15:30")
											   << QDateTime(tomorrow, {16, 00})
											   << QDateTime();
	QTest::newRow("ahead.tomorrow.time.prefix") << QStringLiteral("on tomorrow at 15:30")
												<< QDateTime(today, {15, 00})
												<< QDateTime(tomorrow, {15, 30});
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

void CoreReminderTest::testTimeSpanReminder_data()
{
	QTest::addColumn<QString>("query");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QDateTime>("result");

	//timespan.simple
	QTest::newRow("timespan.minute") << QStringLiteral("in 30 minutes")
									 << QDateTime({2017, 10, 24}, {15, 00})
									 << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.minute.overlap") << QStringLiteral("in 30 minutes")
											 << QDateTime({2017, 10, 24}, {23, 45})
											 << QDateTime({2017, 10, 25}, {0, 15});
	QTest::newRow("timespan.hour") << QStringLiteral("in 2 hours")
								   << QDateTime({2017, 10, 24}, {15, 00})
								   << QDateTime({2017, 10, 24}, {17, 00});
	QTest::newRow("timespan.hour.overlap") << QStringLiteral("in 2 hours")
										   << QDateTime({2017, 10, 24}, {23, 45})
										   << QDateTime({2017, 10, 25}, {1, 45});
	QTest::newRow("timespan.day") << QStringLiteral("in 2 days")
								  << QDateTime({2017, 10, 22})
								  << QDateTime({2017, 10, 24});
	QTest::newRow("timespan.week") << QStringLiteral("in 1 week")
								   << QDateTime({2017, 10, 17})
								   << QDateTime({2017, 10, 24});
	QTest::newRow("timespan.month") << QStringLiteral("in 2 months")
									<< QDateTime({2017, 8, 24})
									<< QDateTime({2017, 10, 24});
	QTest::newRow("timespan.year") << QStringLiteral("in 10 years")
								   << QDateTime({2007, 10, 24})
								   << QDateTime({2017, 10, 24});

	//timespan.time
	QTest::newRow("timespan.minute.time.invalid") << QStringLiteral("in 30 minutes at 15:30")
												  << QDateTime()
												  << QDateTime();
	QTest::newRow("timespan.hour.time.invalid") << QStringLiteral("in 2 hours 17:00")
												<< QDateTime()
												<< QDateTime();
	QTest::newRow("timespan.day.time") << QStringLiteral("in 2 days at 15:30")
									   << QDateTime({2017, 10, 22})
									   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.week.time") << QStringLiteral("in 1 week at 15:30")
										<< QDateTime({2017, 10, 17})
										<< QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.month.time") << QStringLiteral("in 2 months at 15:30")
										 << QDateTime({2017, 8, 24})
										 << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.year.time") << QStringLiteral("in 10 years at 15:30")
										<< QDateTime({2007, 10, 24})
										<< QDateTime({2017, 10, 24}, {15, 30});

	//timespan.date
	QTest::newRow("timespan.minute.date.invalid") << QStringLiteral("in 30 minutes on Tuesday")
												  << QDateTime()
												  << QDateTime();
	QTest::newRow("timespan.hour.date.invalid") << QStringLiteral("in 2 hours in October")
												<< QDateTime()
												<< QDateTime();
	QTest::newRow("timespan.day.date.invalid") << QStringLiteral("in 2 days on Monday")
											   << QDateTime()
											   << QDateTime();
	QTest::newRow("timespan.week.date") << QStringLiteral("in 1 week on Friday")
										<< QDateTime({2017, 10, 17})
										<< QDateTime({2017, 10, 27});
	QTest::newRow("timespan.week.date.after") << QStringLiteral("in 1 week on Monday")
											  << QDateTime({2017, 10, 17})
											  << QDateTime({2017, 10, 23});
	QTest::newRow("timespan.week.date.invalid") << QStringLiteral("in 1 week at 23.")
												<< QDateTime()
												<< QDateTime();
	QTest::newRow("timespan.month.date.weekday") << QStringLiteral("in 2 months on Friday")
												 << QDateTime({2017, 8, 24})
												 << QDateTime({2017, 10, 27});
	QTest::newRow("timespan.month.date.weekday.after") << QStringLiteral("in 2 months on Monday")
													   << QDateTime({2017, 8, 24})
													   << QDateTime({2017, 10, 23});
	QTest::newRow("timespan.month.date.day") << QStringLiteral("in 2 months at 27.")
											 << QDateTime({2017, 8, 24})
											 << QDateTime({2017, 10, 27});
	QTest::newRow("timespan.month.date.day.after") << QStringLiteral("in 2 months at 7.")
												   << QDateTime({2017, 8, 24})
												   << QDateTime({2017, 10, 7});
	QTest::newRow("timespan.month.date.invalid") << QStringLiteral("in 2 months in October")
												 << QDateTime()
												 << QDateTime();
	QTest::newRow("timespan.year.date.weekday") << QStringLiteral("in 10 years on Friday")
												<< QDateTime({2007, 10, 24})
												<< QDateTime({2017, 10, 27});
	QTest::newRow("timespan.year.date.weekday.after") << QStringLiteral("in 10 years on Monday")
													  << QDateTime({2007, 10, 24})
													  << QDateTime({2017, 10, 23});
	QTest::newRow("timespan.year.date.day") << QStringLiteral("in 10 years on 27.")
											<< QDateTime({2007, 10, 24})
											<< QDateTime({2017, 10, 27});
	QTest::newRow("timespan.year.date.day.after") << QStringLiteral("in 10 years on 7.")
												  << QDateTime({2007, 10, 24})
												  << QDateTime({2017, 10, 7});
	QTest::newRow("timespan.year.date.month") << QStringLiteral("in 10 years in December")
											  << QDateTime({2007, 10, 24})
											  << QDateTime({2017, 12, 24});
	QTest::newRow("timespan.year.date.month.after") << QStringLiteral("in 10 years in March")
													<< QDateTime({2007, 10, 24})
													<< QDateTime({2017, 3, 24});
	QTest::newRow("timespan.year.date.monthday") << QStringLiteral("in 10 years on 11.11.")
												 << QDateTime({2007, 10, 24})
												 << QDateTime({2017, 11, 11});
	QTest::newRow("timespan.year.date.monthday.after") << QStringLiteral("in 10 years on 4. July")
													   << QDateTime({2007, 10, 24})
													   << QDateTime({2017, 7, 4});

	//timespan.date.time
	QTest::newRow("timespan.minute.date.time.invalid") << QStringLiteral("in 30 minutes on Tuesday at 15:30")
													   << QDateTime()
													   << QDateTime();
	QTest::newRow("timespan.hour.date.time.invalid") << QStringLiteral("in 2 hours in October at 15:30")
													 << QDateTime()
													 << QDateTime();
	QTest::newRow("timespan.day.date.time.invalid") << QStringLiteral("in 2 days on Monday at 15:30")
													<< QDateTime()
													<< QDateTime();
	QTest::newRow("timespan.week.date.time") << QStringLiteral("in 1 week on Friday at 15:30")
											 << QDateTime({2017, 10, 17}, {15, 00})
											 << QDateTime({2017, 10, 27}, {15, 30});
	QTest::newRow("timespan.week.date.time.after") << QStringLiteral("in 1 week on Monday 15:30")
												   << QDateTime({2017, 10, 17}, {15, 00})
												   << QDateTime({2017, 10, 23}, {15, 30});
	QTest::newRow("timespan.week.date.time.invalid") << QStringLiteral("in 1 week at 23. at 15:30")
													 << QDateTime()
													 << QDateTime();
	QTest::newRow("timespan.month.date.time.weekday") << QStringLiteral("in 2 months on Friday at 15:30")
													  << QDateTime({2017, 8, 24}, {15, 00})
													  << QDateTime({2017, 10, 27}, {15, 30});
	QTest::newRow("timespan.month.date.time.weekday.after") << QStringLiteral("in 2 months on Monday 15:30")
															<< QDateTime({2017, 8, 24}, {15, 00})
															<< QDateTime({2017, 10, 23}, {15, 30});
	QTest::newRow("timespan.month.date.time.day") << QStringLiteral("in 2 months at 27. at 15:30")
												  << QDateTime({2017, 8, 24}, {15, 00})
												  << QDateTime({2017, 10, 27}, {15, 30});
	QTest::newRow("timespan.month.date.time.day.after") << QStringLiteral("in 2 months at 7. 15:30")
														<< QDateTime({2017, 8, 24}, {15, 00})
														<< QDateTime({2017, 10, 7}, {15, 30});
	QTest::newRow("timespan.month.date.time.invalid") << QStringLiteral("in 2 months in October at 15:30")
													  << QDateTime()
													  << QDateTime();
	QTest::newRow("timespan.year.date.time.weekday") << QStringLiteral("in 10 years on Friday at 15:30")
													 << QDateTime({2007, 10, 24}, {15, 00})
													 << QDateTime({2017, 10, 27}, {15, 30});
	QTest::newRow("timespan.year.date.time.weekday.after") << QStringLiteral("in 10 years on Monday 15:30")
														   << QDateTime({2007, 10, 24}, {15, 00})
														   << QDateTime({2017, 10, 23}, {15, 30});
	QTest::newRow("timespan.year.date.time.day") << QStringLiteral("in 10 years on 27. at 15:30")
												 << QDateTime({2007, 10, 24}, {15, 00})
												 << QDateTime({2017, 10, 27}, {15, 30});
	QTest::newRow("timespan.year.date.time.day.after") << QStringLiteral("in 10 years on 7. 15:30")
													   << QDateTime({2007, 10, 24}, {15, 00})
													   << QDateTime({2017, 10, 7}, {15, 30});
	QTest::newRow("timespan.year.date.time.month") << QStringLiteral("in 10 years in December at 15:30")
												   << QDateTime({2007, 10, 24}, {15, 00})
												   << QDateTime({2017, 12, 24}, {15, 30});
	QTest::newRow("timespan.year.date.time.month.after") << QStringLiteral("in 10 years in March 15:30")
														 << QDateTime({2007, 10, 24}, {15, 00})
														 << QDateTime({2017, 3, 24}, {15, 30});
	QTest::newRow("timespan.year.date.time.monthday") << QStringLiteral("in 10 years on 11.11. at 15:30")
													  << QDateTime({2007, 10, 24}, {15, 00})
													  << QDateTime({2017, 11, 11}, {15, 30});
	QTest::newRow("timespan.year.date.time.monthday.after") << QStringLiteral("in 10 years on 4. July 15:30")
															<< QDateTime({2007, 10, 24}, {15, 00})
															<< QDateTime({2017, 7, 4}, {15, 30});
}

void CoreReminderTest::testTimeSpanReminder()
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
