#include "settingsqmlwrapper.h"
#include <syncedsettings.h>

SettingsQmlWrapper::SettingsQmlWrapper(QObject *parent) :
	QObject(parent)
{}

int SettingsQmlWrapper::gui_dateformat() const
{
	return SyncedSettings::instance()->gui.dateformat;
}

void SettingsQmlWrapper::gui_dateformat(int gui_dateformat)
{
	SyncedSettings::instance()->gui.dateformat = gui_dateformat;
}
