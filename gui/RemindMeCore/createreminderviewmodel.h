#ifndef CREATEREMINDERCONTROL_H
#define CREATEREMINDERCONTROL_H

#include <QtMvvmCore/ViewModel>
#include <QtDataSync/DataTypeStore>

#include <eventexpressionparser.h>
#include <remindmelib.h>

class CreateReminderViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool important READ important WRITE setImportant NOTIFY importantChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)
	Q_PROPERTY(bool blocked READ isBlocked NOTIFY blockedChanged)

	QTMVVM_INJECT_PROP(EventExpressionParser*, parser, _parser)

public:
	Q_INVOKABLE explicit CreateReminderViewModel(QObject *parent = nullptr);

	QString text() const;
	bool important() const;
	QString expression() const;
	bool isBlocked() const;

public slots:
	void create();

	void setText(const QString &text);
	void setImportant(bool important);
	void setExpression(const QString &expression);

signals:
	void close();

	void textChanged(const QString &text);
	void importantChanged(bool important);
	void expressionChanged(const QString &expression);
	void blockedChanged(bool blocked);

protected:
	void onResult(quint32 requestCode, const QVariant &result) override;

private:
	static constexpr int TermSelectCode = 10;

	EventExpressionParser *_parser = nullptr;
	ReminderStore *_store;

	QString _text;
	bool _important = false;
	QString _expression;
	bool _blocked = false;

	void finishCreate(const Expressions::MultiTerm &term, const QList<int> & choices = {});
	void setBlocked(bool blocked);
};

#endif // CREATEREMINDERCONTROL_H
