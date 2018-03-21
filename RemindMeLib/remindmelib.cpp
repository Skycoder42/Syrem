#include "remindmelib.h"
#include <QJsonSerializer>
#include <QCoreApplication>
#include <sslremoteobjects.h>

#include "schedule.h"
#include "dateparser.h"
#include "reminder.h"
#include "conflictresolver.h"
#include "snoozetimes.h"

#include <syncedsettings.h>

void RemindMe::setup(QtDataSync::Setup &setup)
{
	setup.setRemoteObjectHost(SslRemoteObjects::generateP12Url(QStringLiteral("127.0.0.1"), 25334,
															   QStringLiteral(":/etc/rocert.p12"),
															   QStringLiteral("baum42")))
			.setSyncPolicy(QtDataSync::Setup::PreferDeleted)
			.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")})
			.setConflictResolver(new ConflictResolver());
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
	qRegisterMetaType<QList<QPair<int, ParserTypes::Expression::Span>>>("QList<QPair<int,ParserTypes::Expression::Span>>");
	qRegisterMetaType<ParserTypes::Datum*>();
	qRegisterMetaType<ParserTypes::Type*>();
	qRegisterMetaType<ParserTypes::TimePoint*>();
	qRegisterMetaType<Schedule*>();
	qRegisterMetaType<OneTimeSchedule*>();
	qRegisterMetaType<LoopSchedule*>();
	qRegisterMetaType<MultiSchedule*>();
	qRegisterMetaType<SnoozeTimes>();
	qRegisterMetaTypeStreamOperators<SnoozeTimes>();

	QJsonSerializer::registerAllConverters<Reminder>();
	QJsonSerializer::registerPointerConverters<Schedule>();
	QJsonSerializer::registerPairConverters<int, ParserTypes::Expression::Span>();
	QJsonSerializer::registerAllConverters<QPair<int, ParserTypes::Expression::Span>>();

	QMetaType::registerConverter<SnoozeTimes, QVariantList>([](const SnoozeTimes &list) -> QVariantList {
		return list.toList();
	});
	QMetaType::registerConverter<QVariantList, SnoozeTimes>([](const QVariantList &list) -> SnoozeTimes {
		SnoozeTimes l;
		for(auto v : list)
			l.append(v.toString());
		return l;
	});
}

}
Q_COREAPP_STARTUP_FUNCTION(setupRemindMeLib)
