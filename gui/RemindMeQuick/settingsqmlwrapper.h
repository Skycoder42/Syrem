#ifndef SETTINGSQMLWRAPPER_H
#define SETTINGSQMLWRAPPER_H

#include <QLocale>
#include <QObject>

class SettingsQmlWrapper : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int gui_dateformat READ gui_dateformat WRITE gui_dateformat NOTIFY gui_dateformatChanged)

public:
	explicit SettingsQmlWrapper(QObject *parent = nullptr);

	int gui_dateformat() const;

public slots:
	void gui_dateformat(int gui_dateformat);

signals:
	void gui_dateformatChanged();
};

#endif // SETTINGSQMLWRAPPER_H
