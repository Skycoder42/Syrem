#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QSortFilterProxyModel>
#include <QtMvvmCore/ViewModel>
#include <QtDataSync/DataStoreModel>
#include <reminder.h>

class MainViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QtDataSync::DataStoreModel* reminderModel READ reminderModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel* sortedModel READ sortedModel CONSTANT)

public:
	static const QString paramRemId;

	Q_INVOKABLE explicit MainViewModel(QObject *parent = nullptr);

	static QVariantHash showParams(const QUuid &reminderId);

	QtDataSync::DataStoreModel* reminderModel() const;
	QSortFilterProxyModel* sortedModel() const;

public slots:
	void showSettings();
	void showSync();
	void showAbout();

	void addReminder();
	void completeReminder(const QUuid &id);
	void deleteReminder(const QUuid &id);
	void snoozeReminder(const QUuid &id);

signals:
	void select(int row);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QtDataSync::DataStoreModel *_reminderModel;
	QSortFilterProxyModel *_sortedModel;
};

#endif // MAINVIEWMODEL_H
