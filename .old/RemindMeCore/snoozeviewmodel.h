#ifndef SNOOZECONTROL_H
#define SNOOZECONTROL_H

#include <QObject>
#include <QUuid>
#include <QtMvvmCore/ViewModel>
class SnoozeHelperReplica;

class SnoozeViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
	Q_PROPERTY(QStringList snoozeTimes READ snoozeTimes NOTIFY snoozeTimesChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
	Q_INVOKABLE explicit SnoozeViewModel(QObject *parent = nullptr);

	static QVariantHash showParams(const QUuid &id, quint32 versionCode);

	bool isLoaded() const;
	QString description() const;
	QStringList snoozeTimes() const;
	QString expression() const;

public slots:
	void snooze();

	void setExpression(QString expression);

signals:
	void close();

	void loadedChanged(bool loaded);
	void descriptionChanged(QString description);
	void snoozeTimesChanged(QStringList snoozeTimes);
	void expressionChanged(QString expression);

protected:
	void onInit(const QVariantHash &params) override;

private slots:
	void reminderLoaded(const QUuid &id, const QString &description);
	void reminderSnoozed(const QUuid &id);
	void reminderError(const QUuid &id, const QString &error, bool close);

private:
	SnoozeHelperReplica *_snoozeHelper;
	bool _loaded;

	QUuid _reminderId;
	QString _description;
	QStringList _snoozeTimes;
	QString _expression;
};

#endif // SNOOZECONTROL_H
