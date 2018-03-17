#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QtMvvmCore/ViewModel>
#include <QtDataSync/DataStoreModel>
#include <reminder.h>

class MainViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QtDataSync::DataStoreModel* reminderModel READ reminderModel CONSTANT)

public:
	Q_INVOKABLE explicit MainViewModel(QObject *parent = nullptr);

	QtDataSync::DataStoreModel* reminderModel() const;

public slots:
	void showSettings();
	void showSync();
	void showAbout();

	void addReminder();
	void completeReminder(const QUuid &id);
	void deleteReminder(const QUuid &id);
	void snoozeReminder(const QUuid &id);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QtDataSync::DataStoreModel *_reminderModel;
};

#endif // MAINVIEWMODEL_H
