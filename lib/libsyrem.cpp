#include "libsyrem.h"
#include <QJsonSerializer>
#include <QCoreApplication>
#include <QtMvvmCore/ServiceRegistry>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QQueue>

#include "schedule.h"
#include "dateparser.h"
#include "reminder.h"
#include "conflictresolver.h"
#include "snoozetimes.h"
#include "eventexpressionparser.h"
#include "terms.h"
#include "termconverter.h"

#include <syncedsettings.h>

namespace {

template <typename TTerm>
void addColumn(QStringList &singleList, QStringList &loopList) {
	using tpr = std::pair<QStringList&, bool>;
	for(auto target : {tpr{singleList, false}, tpr{loopList, true}}) {
		auto info = TTerm::syntax(target.second);
		if(!info.first.isNull()) {
			target.first.append(QStringLiteral("<tr><td>%1</td><td><em>:= %2</em></td></tr>")
						.arg(info.first.toHtmlEscaped(),
							 info.second.toHtmlEscaped()));
		}
	}
}

}

void Syrem::prepareTranslations(const QString &tsName)
{
	//load translations
	auto translator = new QTranslator(qApp);
	if(translator->load(QLocale(),
						tsName,
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
	else {
		qWarning() << "Failed to load translations for locale" << QLocale()
				   << "- Switching to C-locale for a consistent experience";
		delete translator;
		QLocale::setDefault(QLocale::c());
	}
}

void Syrem::setup(QtDataSync::Setup &setup)
{
#ifdef FLATPAK_BUILD
	const QString roPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QLatin1Char('/');
#elif defined(Q_OS_LINUX)
	const QString roPath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + QLatin1Char('/');
#else
	const QString roPath;
#endif
#ifdef QT_NO_DEBUG
	setup.setRemoteObjectHost(QStringLiteral("local:%1de.skycoder42.syrem.daemon").arg(roPath))
			.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")});
#else
	setup.setLocalDir(QStringLiteral(".debug"))
			.setRemoteObjectHost(QStringLiteral("local:%1de.skycoder42.syrem.daemon.debug").arg(roPath))
			.setRemoteConfiguration({QStringLiteral("ws://localhost:14242")});
#endif
	setup.setSyncPolicy(QtDataSync::Setup::PreferDeleted)
			.setConflictResolver(new ConflictResolver{})
			.serializer()->addJsonTypeConverter<TermConverter>();
}

QString Syrem::whenExpressionHelp()
{
	QStringList singularTable;
	QStringList loopTable;
	addColumn<Expressions::TimeTerm>(singularTable, loopTable);
	addColumn<Expressions::DateTerm>(singularTable, loopTable);
	addColumn<Expressions::InvertedTimeTerm>(singularTable, loopTable);
	addColumn<Expressions::MonthDayTerm>(singularTable, loopTable);
	addColumn<Expressions::WeekDayTerm>(singularTable, loopTable);
	addColumn<Expressions::MonthTerm>(singularTable, loopTable);
	addColumn<Expressions::YearTerm>(singularTable, loopTable);
	addColumn<Expressions::SequenceTerm>(singularTable, loopTable);
	addColumn<Expressions::KeywordTerm>(singularTable, loopTable);
	addColumn<Expressions::YearTerm>(singularTable, loopTable);
	addColumn<Expressions::YearTerm>(singularTable, loopTable);
	return QCoreApplication::translate("Syrem",
									   "<h2>Reminder creation help</h2>"
									   "<p>"
									   "	Use the \"when\" field to enter an expression describen when you want to be reminded."
									   "	The app is able to understand common expressions like \"in 3 hours\" or \"in April on the 24th\"."
									   "	The specification below can be used to get an overview of all the possible terms you can enter."
									   "</p><p>"
									   "	<span style=\"text-decoration: underline;\">Pro Tip:</span> "
									   "	If you're unsure if what you enter is correct and correctly understood, "
									   "	you can enable syntax verification in the settings to let the app tell you how it understood what you entered."
									   "</p><p>"
									   "	<h3>Syntax Specification</h3>"
									   "</p><p>"
									   "	Expressions are typically made up of multiple subterms, e.g. \"in April on the 24th\" consits of the subterm \"in April\" and \"on the 24th\"."
									   "	Generally speaking, you can combine any of the supported subterms in any order to create expressions, as long as they are still logical."
									   "	For example, \"at 15:00 in 3 hours\" is not possible, as the two subterms conflict each other."
									   "</p><p>"
									   "	<h4>Possible Subterms</h4>"
									   "</p><p>"
									   "	The following syntaxes are the different subterms available."
									   "	The first table shows all singular expressions, i.e. expression that when evaluted result in a single occurence."
									   "<table><tbody>"
									   "	%1"
									   "</tbody></table>"
									   "</p><p>"
									   "	The next table shows expressions for repeated events."
									   "	Please note that all repeated expression are by default \"infinitely\", i.e. they repeat until all eternity or until they get manually deleted by you."
									   "	However, you can use so called limiter expression to specifiy a range for the to occur."
									   "	The syntax is:&nbsp;<em>{loop-term} [from {limiter-term}] [until|to {limiter-term}]</em>."
									   "	Limiter-terms are just like any normal expression you would enter, but the can't be looped."
									   "	You can specify any limiter in any order. The loop-term is one of the terms from below:"
									   "<table><tbody>"
									   "	%2"
									   "</tbody></table>"
									   "</p><p>"
									   "	<h4>Logical restrains</h4>"
									   "</p><p>"
									   "	While generally speaking, you can combine those terms in any order, as long as they don't conflict each other, there are a few further restrains, "
									   "	some of logical and some of technical origin. They are listed below:"
									   "<ul>"
									   "	<li>There can only be a single loop term per expression</li>"
									   "	<li>Limiters must be unique and \"until\" in the future of \"from\"</li>"
									   "	<li>There can only be a single timespan per expression</li>"
									   "	<li>That timespan must have the greatest scope</li>"
									   "	<li>Limiters cannot be smaller than the scope of the loop expressions fence</li>"
									   "	<li>All expression must evaluate to the future</li>"
									   "	<li>Loops must have at least one valid occurence</li>"
									   "</ul>"
									   "</p><p>"
									   "	<h3>Examples</h3>"
									   "</p><p>"
									   "	<em>&lt;Coming soon...&gt;</em>"
									   "</p>")
			.arg(singularTable.join(QString{}), loopTable.join(QString{}));
}

namespace {

void setupLibSyrem()
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
		l.reserve(list.size());
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

Q_COREAPP_STARTUP_FUNCTION(setupLibSyrem)
