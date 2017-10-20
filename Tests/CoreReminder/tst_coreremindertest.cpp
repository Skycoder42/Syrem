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

	void testLoopReminder_data();
	void testLoopReminder();

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

	//timespan.multi
	QTest::newRow("timespan.multi") << QStringLiteral("in 2 hours and 30 minutes")
									<< QDateTime({2017, 10, 24}, {13, 00})
									<< QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.multi.all") << QStringLiteral("in 1 year and 2 months and 4 weeks and 3 days and 2 hours and 30 minutes")
										<< QDateTime({2016, 7, 23}, {13, 00})
										<< QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.multi.repeat") << QStringLiteral("in 90 minutes and 30 minutes and 30 minutes")
										   << QDateTime({2017, 10, 24}, {13, 00})
										   << QDateTime({2017, 10, 24}, {15, 30});
	QTest::newRow("timespan.multi.date") << QStringLiteral("in 1 year and 2 months on Sunday 15:30")
										 << QDateTime({2016, 8, 24}, {15, 00})
										 << QDateTime({2017, 10, 29}, {15, 30});
	QTest::newRow("timespan.multi.date.invalid") << QStringLiteral("in 1 year and 2 months and 30 minutes on Sunday 15:30")
												 << QDateTime()
												 << QDateTime();
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

void CoreReminderTest::testLoopReminder_data()
{
	QTest::addColumn<QString>("query");
	QTest::addColumn<QDateTime>("since");
	QTest::addColumn<QList<QDateTime>>("results");

	//loop.span
	QTest::newRow("loop.span.minute") << QStringLiteral("every 30 minutes")
									  << QDateTime({2017, 10, 24}, {22, 00})
									  << QList<QDateTime> {
											QDateTime({2017, 10, 24}, {22, 30}),
											QDateTime({2017, 10, 24}, {23, 00}),
											QDateTime({2017, 10, 24}, {23, 30}),
											QDateTime({2017, 10, 25}, {0, 00}),
											QDateTime({2017, 10, 25}, {0, 30}),
											QDateTime({2017, 10, 25}, {1, 00}) //actually infinite, but prooving the point...
										 };
	QTest::newRow("loop.span.minute.time.invalid") << QStringLiteral("every 30 minutes at 15:30")
												   << QDateTime()
												   << QList<QDateTime>();
	QTest::newRow("loop.span.hour") << QStringLiteral("every 2 hours")
									<< QDateTime({2017, 10, 24}, {15, 00})
									<< QList<QDateTime> {
										  QDateTime({2017, 10, 24}, {17, 00}),
										  QDateTime({2017, 10, 24}, {19, 00}),
										  QDateTime({2017, 10, 24}, {21, 00}),
										  QDateTime({2017, 10, 24}, {23, 00}),
										  QDateTime({2017, 10, 25}, {1, 00}),
										  QDateTime({2017, 10, 25}, {3, 00})
									   };
	QTest::newRow("loop.span.hour.time.invalid") << QStringLiteral("every 2 hours 15:30")
												 << QDateTime()
												 << QList<QDateTime>();
	QTest::newRow("loop.span.day") << QStringLiteral("every 2 days")
								   << QDateTime({2017, 10, 22})
								   << QList<QDateTime> {
										 QDateTime({2017, 10, 24}),
										 QDateTime({2017, 10, 26}),
										 QDateTime({2017, 10, 28}),
										 QDateTime({2017, 10, 30}),
										 QDateTime({2017, 11, 1}),
										 QDateTime({2017, 11, 3})
									  };
	QTest::newRow("loop.span.day.time") << QStringLiteral("every 2 days at 15:30")
										<< QDateTime({2017, 10, 22}, {15, 00})
										<< QList<QDateTime> {
											  QDateTime({2017, 10, 24}, {15, 30}),
											  QDateTime({2017, 10, 26}, {15, 30}),
											  QDateTime({2017, 10, 28}, {15, 30}),
											  QDateTime({2017, 10, 30}, {15, 30}),
											  QDateTime({2017, 11, 1}, {15, 30}),
											  QDateTime({2017, 11, 3}, {15, 30})
										   };
	QTest::newRow("loop.span.week") << QStringLiteral("every 1 week")
									<< QDateTime({2017, 10, 17})
									<< QList<QDateTime> {
										  QDateTime({2017, 10, 24}),
										  QDateTime({2017, 10, 31}),
										  QDateTime({2017, 11, 7}),
										  QDateTime({2017, 11, 14})
									   };
	QTest::newRow("loop.span.week.time") << QStringLiteral("every 1 week 15:30")
										 << QDateTime({2017, 10, 17}, {15, 00})
										 << QList<QDateTime> {
											   QDateTime({2017, 10, 24}, {15, 30}),
											   QDateTime({2017, 10, 31}, {15, 30}),
											   QDateTime({2017, 11, 7}, {15, 30}),
											   QDateTime({2017, 11, 14}, {15, 30})
											};
	QTest::newRow("loop.span.month") << QStringLiteral("every 2 months")
									 << QDateTime({2017, 8, 24})
									 << QList<QDateTime> {
										   QDateTime({2017, 10, 24}),
										   QDateTime({2017, 12, 24}),
										   QDateTime({2018, 2, 24}),
										   QDateTime({2018, 4, 24})
										};
	QTest::newRow("loop.span.month.time") << QStringLiteral("every 2 months at 15:30")
										  << QDateTime({2017, 8, 24}, {15, 00})
										  << QList<QDateTime> {
												QDateTime({2017, 10, 24}, {15, 30}),
												QDateTime({2017, 12, 24}, {15, 30}),
												QDateTime({2018, 2, 24}, {15, 30}),
												QDateTime({2018, 4, 24}, {15, 30})
											 };
	QTest::newRow("loop.span.year") << QStringLiteral("every 10 years")
									<< QDateTime({2007, 10, 24})
									<< QList<QDateTime> {
										  QDateTime({2017, 10, 24}),
										  QDateTime({2027, 10, 24}),
										  QDateTime({2037, 10, 24})
									   };
	QTest::newRow("loop.span.year.time") << QStringLiteral("every 10 years 15:30")
										 << QDateTime({2007, 10, 24}, {15, 00})
										 << QList<QDateTime> {
											   QDateTime({2017, 10, 24}, {15, 30}),
											   QDateTime({2027, 10, 24}, {15, 30}),
											   QDateTime({2037, 10, 24}, {15, 30})
											};
	QTest::newRow("loop.span.multi") << QStringLiteral("every 2 hours and 30 minutes")
									  << QDateTime({2017, 10, 24}, {19, 30})
									  << QList<QDateTime> {
											QDateTime({2017, 10, 24}, {22, 00}),
											QDateTime({2017, 10, 25}, {0, 30}),
											QDateTime({2017, 10, 25}, {3, 00}),
											QDateTime({2017, 10, 25}, {5, 30}),
											QDateTime({2017, 10, 25}, {8, 00}),
											QDateTime({2017, 10, 25}, {10, 30})
										 };

	//loop.datum
	QTest::newRow("loop.weekday") << QStringLiteral("every Tuesday")
								  << QDateTime({2017, 10, 20})
								  << QList<QDateTime> {
										QDateTime({2017, 10, 24}),
										QDateTime({2017, 10, 31}),
										QDateTime({2017, 11, 7})
									 };
	QTest::newRow("loop.weekday.same") << QStringLiteral("every Tuesday")
									   << QDateTime({2017, 10, 17})
									   << QList<QDateTime> {
											 QDateTime({2017, 10, 24}),
											 QDateTime({2017, 10, 31}),
											 QDateTime({2017, 11, 7})
										  };
	QTest::newRow("loop.weekday.time") << QStringLiteral("every Tuesday at 15:30")
									   << QDateTime({2017, 10, 20}, {15, 00})
									   << QList<QDateTime> {
											 QDateTime({2017, 10, 24}, {15, 30}),
											 QDateTime({2017, 10, 31}, {15, 30}),
											 QDateTime({2017, 11, 7}, {15, 30})
										  };
	QTest::newRow("loop.day") << QStringLiteral("every 24.")
							  << QDateTime({2017, 10, 20})
							  << QList<QDateTime> {
									QDateTime({2017, 10, 24}),
									QDateTime({2017, 11, 24}),
									QDateTime({2017, 12, 24}),
									QDateTime({2018, 1, 24})
								 };
	QTest::newRow("loop.day.same") << QStringLiteral("every 24.")
								   << QDateTime({2017, 9, 24})
								   << QList<QDateTime> {
										 QDateTime({2017, 10, 24}),
										 QDateTime({2017, 11, 24}),
										 QDateTime({2017, 12, 24}),
										 QDateTime({2018, 1, 24})
									  };
	QTest::newRow("loop.day.time") << QStringLiteral("every 24. 15:30")
								   << QDateTime({2017, 10, 20}, {15, 00})
								   << QList<QDateTime> {
										 QDateTime({2017, 10, 24}, {15, 30}),
										 QDateTime({2017, 11, 24}, {15, 30}),
										 QDateTime({2017, 12, 24}, {15, 30}),
										 QDateTime({2018, 1, 24}, {15, 30})
									  };
	QTest::newRow("loop.month") << QStringLiteral("every October")
								<< QDateTime({2017, 8, 24})
								<< QList<QDateTime> {
									  QDateTime({2017, 10, 24}),
									  QDateTime({2018, 10, 24}),
									  QDateTime({2019, 10, 24})
								   };
	QTest::newRow("loop.month.same") << QStringLiteral("every October")
									 << QDateTime({2016, 10, 24})
									 << QList<QDateTime> {
										   QDateTime({2017, 10, 24}),
										   QDateTime({2018, 10, 24}),
										   QDateTime({2019, 10, 24})
										};
	QTest::newRow("loop.month.time") << QStringLiteral("every October at 15:30")
									 << QDateTime({2017, 8, 24}, {15, 00})
									 << QList<QDateTime> {
										   QDateTime({2017, 10, 24}, {15, 30}),
										   QDateTime({2018, 10, 24}, {15, 30}),
										   QDateTime({2019, 10, 24}, {15, 30})
										};
	QTest::newRow("loop.monthday") << QStringLiteral("every 24.10.")
								   << QDateTime({2017, 8, 24})
								   << QList<QDateTime> {
										 QDateTime({2017, 10, 24}),
										 QDateTime({2018, 10, 24}),
										 QDateTime({2019, 10, 24})
									  };
	QTest::newRow("loop.monthday.same") << QStringLiteral("every 24. October")
										<< QDateTime({2016, 10, 24})
										<< QList<QDateTime> {
											  QDateTime({2017, 10, 24}),
											  QDateTime({2018, 10, 24}),
											  QDateTime({2019, 10, 24})
										   };
	QTest::newRow("loop.monthday.time") << QStringLiteral("every 24.10. 15:30")
										<< QDateTime({2017, 8, 24}, {15, 00})
										<< QList<QDateTime> {
											  QDateTime({2017, 10, 24}, {15, 30}),
											  QDateTime({2018, 10, 24}, {15, 30}),
											  QDateTime({2019, 10, 24}, {15, 30})
										   };

	//loop.span.datum
	QTest::newRow("loop.span.minute.datum.invalid") << QStringLiteral("every 30 minutes on Sunday")
													<< QDateTime()
													<< QList<QDateTime>();
	QTest::newRow("loop.span.hour.datum.invalid") << QStringLiteral("every 2 hours in June")
												  << QDateTime()
												  << QList<QDateTime>();
	QTest::newRow("loop.span.day.datum.invalid") << QStringLiteral("every 2 days on Mondays")
												 << QDateTime()
												 << QList<QDateTime>();
	QTest::newRow("loop.span.week.datum.weekday") << QStringLiteral("every 2 weeks on Friday")
												  << QDateTime({2017, 10, 17})
												  << QList<QDateTime> {
														QDateTime({2017, 11, 3}),
														QDateTime({2017, 11, 17}),
														QDateTime({2017, 12, 1})
													 };
	QTest::newRow("loop.span.week.datum.weekday.time") << QStringLiteral("every 2 weeks on Friday 15:30")
													   << QDateTime({2017, 10, 17}, {15, 00})
													   << QList<QDateTime> {
															 QDateTime({2017, 11, 3}, {15, 30}),
															 QDateTime({2017, 11, 17}, {15, 30}),
															 QDateTime({2017, 12, 1}, {15, 30})
														  };
	QTest::newRow("loop.span.week.datum.invalid") << QStringLiteral("every 2 weeks in May")
												  << QDateTime()
												  << QList<QDateTime>();
	QTest::newRow("loop.span.month.datum.weekday") << QStringLiteral("every 2 months on Tuesday")
												   << QDateTime({2017, 8, 20})
												   << QList<QDateTime> {
														 QDateTime({2017, 10, 17}),
														 QDateTime({2017, 12, 12}),
														 QDateTime({2018, 2, 13}),
														 QDateTime({2018, 4, 10})
													  };
	QTest::newRow("loop.span.month.datum.weekday.time") << QStringLiteral("every 2 months on Tuesday at 15:30")
														<< QDateTime({2017, 8, 20}, {15, 00})
														<< QList<QDateTime> {
															  QDateTime({2017, 10, 17}, {15, 30}),
															  QDateTime({2017, 12, 12}, {15, 30}),
															  QDateTime({2018, 2, 13}, {15, 30}),
															  QDateTime({2018, 4, 10}, {15, 30})
														   };
	QTest::newRow("loop.span.month.datum.day") << QStringLiteral("every 2 months at 24.")
											   << QDateTime({2017, 8, 20})
											   << QList<QDateTime> {
													 QDateTime({2017, 10, 24}),
													 QDateTime({2017, 12, 24}),
													 QDateTime({2018, 2, 24}),
													 QDateTime({2018, 4, 24})
												  };
	QTest::newRow("loop.span.month.datum.day.time") << QStringLiteral("every 2 months at 24. at 15:30")
													<< QDateTime({2017, 8, 20}, {15, 00})
													<< QList<QDateTime> {
														  QDateTime({2017, 10, 24}, {15, 30}),
														  QDateTime({2017, 12, 24}, {15, 30}),
														  QDateTime({2018, 2, 24}, {15, 30}),
														  QDateTime({2018, 4, 24}, {15, 30})
													   };
	QTest::newRow("loop.span.month.datum.invalid") << QStringLiteral("every 2 months at June")
												   << QDateTime()
												   << QList<QDateTime>();
	QTest::newRow("loop.span.year.datum.weekday") << QStringLiteral("every 10 years on Tuesday")
												  << QDateTime({2007, 10, 20})
												  << QList<QDateTime> {
														QDateTime({2017, 10, 17}),
														QDateTime({2027, 10, 12}),
														QDateTime({2037, 10, 13})
													 };
	QTest::newRow("loop.span.year.datum.weekday.time") << QStringLiteral("every 10 years on Tuesday at 15:30")
													   << QDateTime({2007, 10, 20}, {15, 00})
													   << QList<QDateTime> {
															 QDateTime({2017, 10, 17}, {15, 30}),
															 QDateTime({2027, 10, 12}, {15, 30}),
															 QDateTime({2037, 10, 13}, {15, 30})
														  };
	QTest::newRow("loop.span.year.datum.day") << QStringLiteral("every 10 years at 24.")
											  << QDateTime({2007, 10, 20})
											  << QList<QDateTime> {
													QDateTime({2017, 10, 24}),
													QDateTime({2027, 10, 24}),
													QDateTime({2037, 10, 24})
												 };
	QTest::newRow("loop.span.year.datum.day.time") << QStringLiteral("every 10 years at 24. 15:30")
												   << QDateTime({2007, 10, 20}, {15, 00})
												   << QList<QDateTime> {
														 QDateTime({2017, 10, 24}, {15, 30}),
														 QDateTime({2027, 10, 24}, {15, 30}),
														 QDateTime({2037, 10, 24}, {15, 30})
													  };
	QTest::newRow("loop.span.year.datum.month") << QStringLiteral("every 10 years in November")
												<< QDateTime({2007, 5, 31})
												<< QList<QDateTime> {
													  QDateTime({2017, 11, 30}),
													  QDateTime({2027, 11, 30}),
													  QDateTime({2037, 11, 30})
												   };
	QTest::newRow("loop.span.year.datum.month.time") << QStringLiteral("every 10 years in November at 15:30")
													 << QDateTime({2007, 5, 31}, {15, 00})
													 << QList<QDateTime> {
														   QDateTime({2017, 11, 30}, {15, 30}),
														   QDateTime({2027, 11, 30}, {15, 30}),
														   QDateTime({2037, 11, 30}, {15, 30})
														};
	QTest::newRow("loop.span.year.datum.monthday") << QStringLiteral("every 10 years at 24.10.")
												   << QDateTime({2007, 5, 31})
												   << QList<QDateTime> {
														 QDateTime({2017, 10, 24}),
														 QDateTime({2027, 10, 24}),
														 QDateTime({2037, 10, 24})
													  };
	QTest::newRow("loop.span.year.datum.monthday.time") << QStringLiteral("every 10 years at 24. October 15:30")
														<< QDateTime({2007, 5, 31}, {15, 00})
														<< QList<QDateTime> {
															  QDateTime({2017, 10, 24}, {15, 30}),
															  QDateTime({2027, 10, 24}, {15, 30}),
															  QDateTime({2037, 10, 24}, {15, 30})
														   };
	QTest::newRow("loop.span.multi.datum.weekday") << QStringLiteral("every 2 years and 3 months on Friday")
												   << QDateTime({2015, 5, 24})
												   << QList<QDateTime> {
														 QDateTime({2017, 8, 25}),
														 QDateTime({2019, 11, 29}),
														 QDateTime({2022, 3, 4})
													  };
	QTest::newRow("loop.span.multi.datum.weekday.time") << QStringLiteral("every 2 years and 3 months on Friday at 15:30")
														<< QDateTime({2015, 5, 24}, {15, 00})
														<< QList<QDateTime> {
															  QDateTime({2017, 8, 25}, {15, 30}),
															  QDateTime({2019, 11, 29}, {15, 30}),
															  QDateTime({2022, 3, 4}, {15, 30})
														   };
	QTest::newRow("loop.span.multi.datum.invalid") << QStringLiteral("every 2 years and 3 months and 30 minutes on Friday")
												   << QDateTime()
												   << QList<QDateTime>();

	//loop.datum.datum
	QTest::newRow("loop.weekday.datum.invalid") << QStringLiteral("every Tuesday on Sunday")
												<< QDateTime()
												<< QList<QDateTime>();
	QTest::newRow("loop.day.datum.invalid") << QStringLiteral("every 24. on Friday")
											<< QDateTime()
											<< QList<QDateTime>();
	QTest::newRow("loop.month.datum.weekday") << QStringLiteral("every October on Friday")
											  << QDateTime({2017, 8, 24})
											  << QList<QDateTime> {
													QDateTime({2017, 10, 27}),
													QDateTime({2018, 10, 26}),
													QDateTime({2019, 10, 25})
												 };
	QTest::newRow("loop.month.datum.weekday.time") << QStringLiteral("every October on Friday at 15:30")
												   << QDateTime({2017, 8, 24}, {15, 00})
												   << QList<QDateTime> {
														 QDateTime({2017, 10, 27}, {15, 30}),
														 QDateTime({2018, 10, 26}, {15, 30}),
														 QDateTime({2019, 10, 25}, {15, 30})
													  };
	QTest::newRow("loop.month.datum.day") << QStringLiteral("every October at 24.")
										  << QDateTime({2017, 8, 20})
										  << QList<QDateTime> {
												QDateTime({2017, 10, 24}),
												QDateTime({2018, 10, 24}),
												QDateTime({2019, 10, 24})
											 };
	QTest::newRow("loop.month.datum.day.time") << QStringLiteral("every October at 24. 15:30")
											   << QDateTime({2017, 8, 20}, {15, 00})
											   << QList<QDateTime> {
													 QDateTime({2017, 10, 24}, {15, 30}),
													 QDateTime({2018, 10, 24}, {15, 30}),
													 QDateTime({2019, 10, 24}, {15, 30})
												  };
	QTest::newRow("loop.month.datum.invalid") << QStringLiteral("every October in August")
											  << QDateTime()
											  << QList<QDateTime>();
	QTest::newRow("loop.monthday.datum.invalid") << QStringLiteral("every 24.10. on Friday")
												 << QDateTime()
												 << QList<QDateTime>();

	//loop.span.from
	QTest::newRow("loop.span.from") << QStringLiteral("every 30 minutes from 25. October")
									<< QDateTime({2017, 10, 24}, {22, 00})
									<< QList<QDateTime> {
										  QDateTime({2017, 10, 25}, {00, 30}),
										  QDateTime({2017, 10, 25}, {1, 00}),
										  QDateTime({2017, 10, 25}, {1, 30}),
										  QDateTime({2017, 10, 25}, {2, 00}),
										  QDateTime({2017, 10, 25}, {2, 30}),
										  QDateTime({2017, 10, 25}, {3, 00}),
										  QDateTime({2017, 10, 25}, {3, 30})
									   };
	QTest::newRow("loop.span.from.time") << QStringLiteral("every 30 minutes from 25. October at 21:00")
										 << QDateTime({2017, 10, 24}, {22, 00})
										 << QList<QDateTime> {
											   QDateTime({2017, 10, 25}, {21, 30}),
											   QDateTime({2017, 10, 25}, {22, 00}),
											   QDateTime({2017, 10, 25}, {22, 30}),
											   QDateTime({2017, 10, 25}, {23, 00}),
											   QDateTime({2017, 10, 25}, {23, 30}),
											   QDateTime({2017, 10, 26}, {0, 00}),
											   QDateTime({2017, 10, 26}, {0, 30})
											};
	QTest::newRow("loop.span.from.datum") << QStringLiteral("every 2 weeks on Friday from 24.10.2017")
										  << QDateTime({2010, 10, 24})
										  << QList<QDateTime> {
												QDateTime({2017, 11, 10}),
												QDateTime({2017, 11, 24}),
												QDateTime({2017, 12, 8})
											 };
	QTest::newRow("loop.span.from.time.datum") << QStringLiteral("every 2 weeks on Friday from 24.10.2017 15:30")
											   << QDateTime({2010, 10, 24}, {15, 00})
											   << QList<QDateTime> {
													 QDateTime({2017, 11, 10}, {15, 30}),
													 QDateTime({2017, 11, 24}, {15, 30}),
													 QDateTime({2017, 12, 8}, {15, 30})
												  };
	QTest::newRow("loop.span.from.datum.time") << QStringLiteral("every 2 weeks on Friday at 15:30 from 24.10.2017")
											   << QDateTime({2010, 10, 24}, {15, 00})
											   << QList<QDateTime> {
													 QDateTime({2017, 11, 10}, {15, 30}),
													 QDateTime({2017, 11, 24}, {15, 30}),
													 QDateTime({2017, 12, 8}, {15, 30})
												  };
	QTest::newRow("loop.span.from.time.datum.time") << QStringLiteral("every 2 weeks on Friday at 15:30 from 24.10.2017 at 17:00")
													<< QDateTime({2010, 10, 24}, {15, 00})
													<< QList<QDateTime> {
														  QDateTime({2017, 11, 10}, {15, 30}),
														  QDateTime({2017, 11, 24}, {15, 30}),
														  QDateTime({2017, 12, 8}, {15, 30})
													   };
	QTest::newRow("loop.span.from.datum.invalid") << QStringLiteral("every 2 weeks on October from 24.10.2017")
												  << QDateTime()
												  << QList<QDateTime>();

	//loop.datum.from
	QTest::newRow("loop.datum.from") << QStringLiteral("every Friday from 25. October")
									 << QDateTime({2017, 10, 18})
									 << QList<QDateTime> {
										   QDateTime({2017, 10, 27}),
										   QDateTime({2017, 11, 3}),
										   QDateTime({2017, 11, 10})
										};
	QTest::newRow("loop.datum.from.time") << QStringLiteral("every Friday from 25. October at 15:30")
										  << QDateTime({2017, 10, 18}, {15, 00})
										  << QList<QDateTime> {
												QDateTime({2017, 10, 27}, {15, 30}),
												QDateTime({2017, 11, 3}, {15, 30}),
												QDateTime({2017, 11, 10}, {15, 30})
											 };
	QTest::newRow("loop.datum.from.datum") << QStringLiteral("every October on 25. from 28.")
										   << QDateTime({2017, 9, 18})
										   << QList<QDateTime> {
												 QDateTime({2017, 10, 25}),
												 QDateTime({2018, 10, 25}),
												 QDateTime({2019, 10, 25}),
												 QDateTime({2020, 10, 25})
											  };
	QTest::newRow("loop.datum.from.time.datum") << QStringLiteral("every October on 25. from 28. 15:30")
												<< QDateTime({2017, 10, 18}, {15, 00})
												<< QList<QDateTime> {
													  QDateTime({2018, 10, 25}, {15, 30}),
													  QDateTime({2019, 10, 25}, {15, 30}),
													  QDateTime({2020, 10, 25}, {15, 30})
												   };
	QTest::newRow("loop.datum.from.datum.time") << QStringLiteral("every October on 25. at 15:30 from 25.")
												<< QDateTime({2017, 10, 18}, {15, 00})
												<< QList<QDateTime> {
													  QDateTime({2018, 10, 25}, {15, 30}),
													  QDateTime({2019, 10, 25}, {15, 30}),
													  QDateTime({2020, 10, 25}, {15, 30})
												   };
	QTest::newRow("loop.datum.from.time.datum.time") << QStringLiteral("every October on 25. at 15:30 from 25. at 17:00")
													 << QDateTime({2017, 10, 18}, {15, 00})
													 << QList<QDateTime> {
														   QDateTime({2018, 10, 25}, {15, 30}),
														   QDateTime({2019, 10, 25}, {15, 30}),
														   QDateTime({2020, 10, 25}, {15, 30})
														};
	QTest::newRow("loop.span.from.datum.invalid") << QStringLiteral("every October in April from tomorrow")
												  << QDateTime()
												  << QList<QDateTime>();
}

void CoreReminderTest::testLoopReminder()
{
	QFETCH(QString, query);
	QFETCH(QDateTime, since);
	QFETCH(QList<QDateTime>, results);

	auto expr = parser->parse(query);
	if(since.isValid()) {
		QVERIFY(expr);
		auto sched = expr->createSchedule(since, this);
		if(!results.isEmpty()) {
			QVERIFY(sched);
			QVERIFY(sched->isRepeating());
			auto next = since;
			while(!results.isEmpty()) {
				auto goal = results.takeFirst();
				next = sched->nextSchedule(next);
				QCOMPARE(next, goal);
			}
			sched->deleteLater();
		} else
			QVERIFY(!sched);
		expr->deleteLater();
	} else
		QVERIFY(!expr);
}

QTEST_MAIN(CoreReminderTest)

#include "tst_coreremindertest.moc"
