#include <QGuiApplication>
#include <QIcon>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmQuick/QuickPresenter>
#include <QtMvvmDataSyncQuick/qtmvvmdatasyncquick_global.h>
#include <createreminderviewmodel.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <snoozeviewmodel.h>
#include <snoozetimes.h>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include "androidscheduler.h"
#include "androidnotifier.h"
#endif

QTMVVM_REGISTER_CORE_APP(RemindMeApp)

static void setupApp();
static void setupDaemon();
static void setStatusBarColor(QColor color);

int main(int argc, char *argv[])
{
	if(qstrcmp(argv[1], "--daemon") == 0) {
		qCritical("PUSSY: in service part");
		QCoreApplication app(argc, argv);
		//workarond
#ifdef Q_OS_ANDROID
		qputenv("PLUGIN_KEYSTORES_PATH", QCoreApplication::applicationDirPath().toUtf8());
		qCritical() << qgetenv("PLUGIN_KEYSTORES_PATH");
#endif
		qCritical("PUSSY: core app created");
		setupDaemon();
		qCritical("PUSSY: daemon part created");
		return app.exec();
	} else {
		QtMvvm::CoreApp::disableAutoBoot();
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
		setStatusBarColor(QColor(0x51, 0x2D, 0xA8));//see qtquickcontrols2.conf

		QGuiApplication app(argc, argv);
		QGuiApplication::setApplicationName(QStringLiteral(TARGET));
		QGuiApplication::setApplicationVersion(QStringLiteral(VERSION));
		QGuiApplication::setOrganizationName(QStringLiteral(COMPANY));
		QGuiApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
		QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
		QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));

		QCommandLineParser parser;
		coreApp->setupParser(parser);
		if(!parser.parse(QCoreApplication::arguments()))
			return EXIT_FAILURE;
		if(parser.isSet(QStringLiteral("daemon")))
			setupDaemon();
		else
			setupApp();

		return app.exec();
	}
}

static void setupApp()
{
	qmlRegisterUncreatableType<MainViewModel>("de.skycoder42.remindme", 1, 0, "MainViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<CreateReminderViewModel>("de.skycoder42.remindme", 1, 0, "CreateReminderViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<SnoozeViewModel>("de.skycoder42.remindme", 1, 0, "SnoozeViewModel", QStringLiteral("ViewModels cannot be created!"));

	QtMvvm::registerDataSyncQuick();

	auto engine = new QQmlApplicationEngine(qApp);
	engine->load(QUrl(QStringLiteral("qrc:/qml/App.qml")));
	auto qPres = dynamic_cast<QtMvvm::QuickPresenter*>(QtMvvm::ServiceRegistry::instance()->service<QtMvvm::IPresenter>());
	qPres->inputViewFactory()->addSimpleInput<QTime>(QStringLiteral("qrc:/qml/inputs/TimeEdit.qml"));
	qPres->inputViewFactory()->addSimpleInput<SnoozeTimes>(QStringLiteral("qrc:/qml/inputs/SnoozeTimesEdit.qml"));

	QMetaObject::invokeMethod(coreApp, "bootApp", Qt::QueuedConnection);

#ifdef Q_OS_ANDROID
	AndroidNotifier::guiStarted();
#endif
}

static void setupDaemon()
{
#ifdef Q_OS_ANDROID
	QtMvvm::ServiceRegistry::instance()->registerInterface<IScheduler, AndroidScheduler>();
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, AndroidNotifier>();
#endif

	auto daemon = new RemindMeDaemon(qApp);
	daemon->startDaemon();

#ifdef Q_OS_ANDROID
	AndroidNotifier::serviceStarted();
#endif
}

static void setStatusBarColor(QColor color)
{
#ifdef Q_OS_ANDROID
	if(QtAndroid::androidSdkVersion() >= 21) {
		auto activity = QtAndroid::androidActivity();
		if(activity.isValid()) {
			QtAndroid::runOnAndroidThreadSync([=](){
				const auto FLAG_TRANSLUCENT_STATUS = QAndroidJniObject::getStaticField<jint>("android/view/WindowManager$LayoutParams",
																							 "FLAG_TRANSLUCENT_STATUS");
				const auto FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS = QAndroidJniObject::getStaticField<jint>("android/view/WindowManager$LayoutParams",
																									   "FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS");
				const auto jColor = QAndroidJniObject::callStaticMethod<jint>("android/graphics/Color",
																			  "parseColor",
																			  "(Ljava/lang/String;)I",
																			  QAndroidJniObject::fromString(color.name()).object());

				QAndroidJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
				if(window.isValid()) {
					window.callMethod<void>("clearFlags", "(I)V", FLAG_TRANSLUCENT_STATUS);
					window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
					window.callMethod<void>("setStatusBarColor", "(I)V", jColor);
				}
			});
		}
	}
#else
	Q_UNUSED(color);
#endif
}
