#ifndef CREATEREMINDERCONTROL_H
#define CREATEREMINDERCONTROL_H

#include <control.h>
class ReminderManagerReplica;

class CreateReminderControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool important READ important WRITE setImportant NOTIFY importantChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
	explicit CreateReminderControl(QObject *parent = nullptr);

	QString text() const;
	bool important() const;
	QString expression() const;

public slots:
	void setText(QString text);
	void setImportant(bool important);
	void setExpression(QString expression);

	void create();

signals:
	void textChanged(QString text);
	void importantChanged(bool important);
	void expressionChanged(QString expression);

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
