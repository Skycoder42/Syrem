#ifndef ANDROIDSCHEDULER_H
#define ANDROIDSCHEDULER_H

#include <QObject>
#include <ischeduler.h>

class AndroidScheduler : public QObject, public IScheduler
{
	Q_OBJECT
	Q_INTERFACES(IScheduler)

public:
	explicit AndroidScheduler(QObject *parent = nullptr);

public slots:
	void initialize(const QList<Reminder> &allReminders) override;
	bool scheduleReminder(const QUuid &id, quint32 versionCode, const QDateTime &timepoint) override;
	void cancleReminder(const QUuid &id) override;

signals:
	void scheduleTriggered(const QUuid &id) final;
};

#endif // ANDROIDSCHEDULER_H
