#ifndef CREATEREMINDERCONTROL_H
#define CREATEREMINDERCONTROL_H

#include <QtMvvmCore/ViewModel>
#include <QtDataSync/DataTypeStore>

#include <dateparser.h>
#include <remindmelib.h>

class CreateReminderViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool important READ important WRITE setImportant NOTIFY importantChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

	QTMVVM_INJECT_PROP(DateParser*, parser, _parser)

public:
	Q_INVOKABLE explicit CreateReminderViewModel(QObject *parent = nullptr);

	QString text() const;
	bool important() const;
	QString expression() const;

	Q_INVOKABLE bool create();

public slots:
	void setText(const QString &text);
	void setImportant(bool important);
	void setExpression(const QString &expression);

signals:
	void close();

	void textChanged(const QString &text);
	void importantChanged(bool important);
	void expressionChanged(const QString &expression);

private:
	DateParser *_parser = nullptr;
	ReminderStore *_store;

	QString _text;
	bool _important = false;
	QString _expression;
};

#endif // CREATEREMINDERCONTROL_H
