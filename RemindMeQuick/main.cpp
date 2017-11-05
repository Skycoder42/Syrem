#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <createremindercontrol.h>
#include <quickpresenter.h>
#include <registry.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <snoozecontrol.h>
#include <snoozetimes.h>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include "androidscheduler.h"
#include "androidnotifier.h"
#endif

REGISTER_CORE_APP(RemindMeApp)

static void setupApp();
static void setupDaemon();
static void setStatusBarColor(QColor color);

int main(int argc, char *argv[])
{
	CoreApp::disableBoot();
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	setStatusBarColor(QColor(0x51, 0x2D, 0xA8));//see qtquickcontrols2.conf

	QGuiApplication app(argc, argv);
	QGuiApplication::setApplicationName(QStringLiteral(TARGET));
	QGuiApplication::setApplicationVersion(QStringLiteral(VERSION));
	QGuiApplication::setOrganizationName(QStringLiteral(COMPANY));
	QGuiApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));

	auto parser = coreApp->getParser();
	if(parser->isSet(QStringLiteral("daemon")))
		setupDaemon();
	else
		setupApp();

	return app.exec();
}

static void setupApp()
{
	qmlRegisterUncreatableType<MainControl>("de.skycoder42.remindme", 1, 0, "MainControl", QStringLiteral("Controls cannot be created!"));
	qmlRegisterUncreatableType<CreateReminderControl>("de.skycoder42.remindme", 1, 0, "CreateReminderControl", QStringLiteral("Controls cannot be created!"));
	qmlRegisterUncreatableType<SnoozeControl>("de.skycoder42.remindme", 1, 0, "SnoozeControl", QStringLiteral("Controls cannot be created!"));

	auto engine = QuickPresenter::createAppEngine(QStringLiteral("qrc:/qml/App.qml"));
	QuickPresenter::inputViewFactory()->addSimpleView<QTime>(QStringLiteral("qrc:/qml/inputs/TimeEdit.qml"));
	QuickPresenter::inputViewFactory()->addSimpleView<SnoozeTimes>(QStringLiteral("qrc:/qml/inputs/SnoozeTimesEdit.qml"));
	engine->rootContext()->setContextProperty(QStringLiteral("qtVersion"), QStringLiteral(QT_VERSION_STR));

	QMetaObject::invokeMethod(coreApp, "bootApp", Qt::QueuedConnection);

#ifdef Q_OS_ANDROID
	AndroidNotifier::guiStarted();
#endif
}

static void setupDaemon()
{
#ifdef Q_OS_ANDROID
	Registry::registerClass<IScheduler, AndroidScheduler>();
	Registry::registerClass<INotifier, AndroidNotifier>();
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
		QtAndroid::runOnAndroidThreadSync([=](){
			auto activity = QtAndroid::androidActivity();
			if(activity.isValid()) {
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
			}
		});
	}
#else
	Q_UNUSED(color);
#endif
}
