#include "remindmelib.h"
#include <QJsonSerializer>
#include <QCoreApplication>
#include <QtMvvmCore/ServiceRegistry>
#include <QTranslator>
#include <QLibraryInfo>

#include "schedule.h"
#include "dateparser.h"
#include "reminder.h"
#include "conflictresolver.h"
#include "snoozetimes.h"
#include "eventexpressionparser.h"
#include "terms.h"
#include "termconverter.h"

#include <syncedsettings.h>

void RemindMe::prepareTranslations(const QString &tsName)
{
	//load translations
	auto translator = new QTranslator(qApp);
	if(translator->load(QLocale(),
						tsName,
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
	else {
		qWarning() << "Failed to load translations! Switching to C-locale for a consistent experience";
		delete translator;
		QLocale::setDefault(QLocale::c());
	}
}

void RemindMe::setup(QtDataSync::Setup &setup)
{
#ifdef QT_NO_DEBUG
	setup.setRemoteObjectHost(QStringLiteral("local:de.skycoder42.remindme.daemon"))
			.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")});
#else
	setup.setLocalDir(QStringLiteral(".debug"))
			.setRemoteObjectHost(QStringLiteral("local:de.skycoder42.remindme.daemon.debug"))
			.setRemoteConfiguration({QStringLiteral("ws://localhost:14242")});
#endif
	setup.setSyncPolicy(QtDataSync::Setup::PreferDeleted)
			.setConflictResolver(new ConflictResolver{})
			.serializer()->addJsonTypeConverter<TermConverter>();
}

QString RemindMe::whenExpressionHelp()
{
	return QCoreApplication::translate("RemindMe",
									   "<p><u>Syntax Specification:</u></p>"
									   "<p>You can enter an <i>&lt;expression&gt;</i> to define timepoints to remind you. An expression can be: </p>"
									   "<p><table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">"
									   " <tr>"
									   "  <td><b>conjuction </b></td>"
									   "  <td>:= <i>&lt;expression&gt; ; &lt;expression&gt; [; &lt;expression&gt; …]</i></td>"
									   " </tr><tr>"
									   "  <td><b>timespan </span></td>"
									   "  <td>:= <i>in &lt;sequence&gt; [on|at|in &lt;datum&gt;] [&lt;time&gt;]</i></td>"
									   " </tr><tr>"
									   "  <td><b>loop </b></td>"
									   "  <td>:= <i>every &lt;type&gt; [on|at|in &lt;datum&gt;] [&lt;time&gt;] [from [&lt;tpoint&gt;] [&lt;time&gt;]] [until [&lt;tpoint&gt;] [&lt;time&gt;]]</i></td>"
									   " </tr><tr>"
									   "  <td><b>point </b></td>"
									   "  <td>:= <i>[[on|next] &lt;tpoint&gt;] [&lt;time&gt;]</i></td>"
									   " </tr>"
									   "</table></p>"
									   "<p><u>Basic Types:</u><br/>"
									   "The specifications above make use of a bunch of basic types. These types are: </p>"
									   "<p><table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">"
									   " <tr>"
									   "  <td><b>datum </b></td>"
									   "  <td>:= <i>&lt;weekday&gt; | &lt;day&gt; | &lt;month&gt; | &lt;mday&gt;</i></td>"
									   " </tr><tr>"
									   "  <td><b>sequence </b></td>"
									   "  <td>:= <i>{int} &lt;span&gt; [and {int} &lt;span&gt; …]</i></td>"
									   " </tr><tr>"
									   "  <td><b>type </b></td>"
									   "  <td>:= <i>&lt;datum&gt; | &lt;sequence&gt;</i></td>"
									   " </tr><tr>"
									   "  <td><b>tpoint </b></td>"
									   "  <td>:= <i>&lt;date&gt; | &lt;datum&gt; | &lt;year&gt; | &lt;ahead&gt;</i></td>"
									   " </tr>"
									   "</table></p>"
									   "<p><table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">"
									   " <tr>"
									   "  <td><b>time </b></td>"
									   "  <td>:= <i>[at] {hh[:mm]} | {hh] oclock</i></td>"
									   " </tr><tr>"
									   "  <td><b>date </b></td>"
									   "  <td>:= <i>{dd-MM-yyyy}</i></td>"
									   " </tr><tr>"
									   "  <td><b>weekday </b></td>"
									   "  <td>:= <i>{Monday..Sunday}</i></td>"
									   " </tr><tr>"
									   "  <td><b>day </b></td>"
									   "  <td>:= <i>{01..31}.</i></td>"
									   " </tr><tr>"
									   "  <td><b>month </b></td>"
									   "  <td>:= <i>{01..12} | {Janurary..December}</i></td>"
									   " </tr><tr>"
									   "  <td><b>year </b></td>"
									   "  <td>:= <i>yyyy</i></td>"
									   " </tr><tr>"
									   "  <td><b>mday </b></td>"
									   "  <td>:= <i>{dd-MM}</i></td>"
									   " </tr><tr>"
									   "  <td><b>span </b></td>"
									   "  <td>:= <i>minute | hour | day | week | month | year</i></td>"
									   " </tr><tr>"
									   "  <td><b>ahead </b></td>"
									   "  <td>:= <i>tomorrow | today</i></td>"
									   " </tr>"
									   "</table></p>"
									   "<p><u>Custom Formats:</u><br/>"
									   "For many of the formats above, you can customize the keywords to be used in the settings. For the &lt;date&gt; and &lt;mday&gt; formats, "
									   "you can even define custom formats to be accepted. Check the settings for details on those formats.</p>");
}

namespace {

void setupRemindMeLib()
{
	qRegisterMetaType<Expressions::SubTerm*>();
	qRegisterMetaType<Expressions::TimeTerm*>();
	qRegisterMetaType<Expressions::DateTerm*>();
	qRegisterMetaType<Expressions::InvertedTimeTerm*>();
	qRegisterMetaType<Expressions::MonthDayTerm*>();
	qRegisterMetaType<Expressions::WeekDayTerm*>();
	qRegisterMetaType<Expressions::MonthTerm*>();
	qRegisterMetaType<Expressions::YearTerm*>();
	qRegisterMetaType<Expressions::SequenceTerm*>();
	qRegisterMetaType<Expressions::KeywordTerm*>();
	qRegisterMetaType<Expressions::LimiterTerm*>();

	qRegisterMetaType<Schedule*>();
	qRegisterMetaType<SingularSchedule*>();
	qRegisterMetaType<RepeatedSchedule*>();
	qRegisterMetaType<MultiSchedule*>();

	qRegisterMetaType<SnoozeTimes>();
	qRegisterMetaTypeStreamOperators<SnoozeTimes>();

	QJsonSerializer::registerAllConverters<Reminder>();

	QJsonSerializer::registerAllConverters<Schedule*>();
	QJsonSerializer::registerPointerConverters<Schedule>();
	QJsonSerializer::registerAllConverters<QSharedPointer<Schedule>>();

	QJsonSerializer::registerAllConverters<Expressions::SubTerm*>();
	QJsonSerializer::registerPointerConverters<Expressions::SubTerm>();
	QJsonSerializer::registerAllConverters<QSharedPointer<Expressions::SubTerm>>();

	QMetaType::registerConverter<SnoozeTimes, QVariantList>([](const SnoozeTimes &list) -> QVariantList {
		return list.toList();
	});
	QMetaType::registerConverter<QVariantList, SnoozeTimes>([](const QVariantList &list) -> SnoozeTimes {
		SnoozeTimes l;
		for(const auto& v : list)
			l.append(v.toString());
		return l;
	});

	// old types
	qRegisterMetaType<QList<QPair<int, ParserTypes::Expression::Span>>>("QList<QPair<int,ParserTypes::Expression::Span>>");
	qRegisterMetaType<ParserTypes::Datum*>();
	qRegisterMetaType<ParserTypes::Type*>();
	qRegisterMetaType<ParserTypes::TimePoint*>();
	qRegisterMetaType<OneTimeSchedule*>();
	qRegisterMetaType<LoopSchedule*>();
	QJsonSerializer::registerPairConverters<int, ParserTypes::Expression::Span>();
	QJsonSerializer::registerAllConverters<QPair<int, ParserTypes::Expression::Span>>();

	QtMvvm::ServiceRegistry::instance()->registerObject<EventExpressionParser>();
}

}

Q_COREAPP_STARTUP_FUNCTION(setupRemindMeLib)
