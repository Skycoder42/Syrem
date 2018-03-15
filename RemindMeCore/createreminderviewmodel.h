#ifndef CREATEREMINDERCONTROL_H
#define CREATEREMINDERCONTROL_H

#include <QtMvvmCore/ViewModel>
class ReminderManagerReplica;

class CreateReminderViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool important READ important WRITE setImportant NOTIFY importantChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
	Q_INVOKABLE explicit CreateReminderViewModel(QObject *parent = nullptr);

	QString text() const;
	bool important() const;
	QString expression() const;

public slots:
	void setText(const QString &text);
	void setImportant(bool important);
	void setExpression(const QString &expression);

	void create();

signals:
	void textChanged(const QString &text);
	void importantChanged(bool important);
	void expressionChanged(const QString &expression);

	void createCompleted(bool success);

private slots:
	void remCreated();
	void remError(bool isCreate, const QString &error);

private:
	ReminderManagerReplica *_reminderManager;

	QString _text;
	bool _important;
	QString _expression;
};

#endif // CREATEREMINDERCONTROL_H
