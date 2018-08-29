#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QColor>
#include <QtMvvmQuick/QuickPresenter>
#include <QtMvvmDataSyncQuick/qtmvvmdatasyncquick_global.h>
#include <remindmeapp.h>
#include <mainviewmodel.h>
#include <createreminderviewmodel.h>
#include <snoozeviewmodel.h>
#include <snoozetimes.h>
#include <termselectionviewmodel.h>
#include <qml_syncedsettings.h>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

QTMVVM_REGISTER_CORE_APP(RemindMeApp)

namespace {

QObject *create_snooze_times_generator(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
	return new SnoozeTimesGenerator(qmlEngine);
}

void setStatusBarColor(QColor color)
{
#ifdef Q_OS_ANDROID
	if(QtAndroid::androidSdkVersion() >= 21) {
		auto activity = QtAndroid::androidActivity();
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
#else
	Q_UNUSED(color);
#endif
}

}

int main(int argc, char *argv[])
{
	setStatusBarColor(QColor(0x51, 0x2D, 0xA8));//see qtquickcontrols2.conf

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterUncreatableType<SnoozeTimes>("de.skycoder42.remindme", 1, 0, "SnoozeTimes", QStringLiteral("Q_GADGETs cannot be created!"));
	QMLTYPE_SyncedSettings::registerQmlTypes("de.skycoder42.remindme", 1, 0);
	qmlRegisterSingletonType<SnoozeTimesGenerator>("de.skycoder42.remindme", 1, 0, "SnoozeTimesGenerator", create_snooze_times_generator);
	qmlRegisterUncreatableType<Reminder>("de.skycoder42.remindme", 1, 0, "Reminder", QStringLiteral("Q_GADGETs cannot be created!"));
	qmlRegisterUncreatableType<MainViewModel>("de.skycoder42.remindme", 1, 0, "MainViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<CreateReminderViewModel>("de.skycoder42.remindme", 1, 0, "CreateReminderViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<SnoozeViewModel>("de.skycoder42.remindme", 1, 0, "SnoozeViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<TermSelectionViewModel>("de.skycoder42.remindme", 1, 0, "TermSelectionViewModel", QStringLiteral("ViewModels cannot be created!"));

	QtMvvm::registerDataSyncQuick();
	QtMvvm::QuickPresenter::getInputViewFactory()->addSimpleInput<SnoozeTimes>(QStringLiteral("qrc:/qtmvvm/inputs/SnoozeTimesEdit.qml"));

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty(QStringLiteral("coreApp"), coreApp);
	engine.load(QUrl(QStringLiteral("qrc:/qml/App.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
