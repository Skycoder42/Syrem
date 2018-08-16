#ifndef EVENTEXPRESSIONPARSER_H
#define EVENTEXPRESSIONPARSER_H

#include <QObject>
#include <QtMvvmCore/Injection>

#include "remindmelib_global.h"
#include <syncedsettings.h>

namespace Expressions {

enum TypeFlag {
	InvalidType = 0x00,

	FlagAbsolute = 0x01,
	FlagRelative = 0x02,
	FlagTimepoint = 0x04,
	FlagTimespan = 0x08,
	FlagLooped = 0x10,

	AbsoluteTimepoint = FlagAbsolute | FlagTimepoint,
	RelativeTimepoint = FlagRelative | FlagTimepoint,
	Timespan = FlagRelative | FlagTimespan,
	LoopedPoint = RelativeTimepoint | FlagLooped,
	LoopedSpan = Timespan | FlagLooped
};
Q_DECLARE_FLAGS(Type, TypeFlag)

enum ScopeFlag {
	InvalidScope = 0x00,

	Year = 0x01,
	Month = 0x02,
	Week = 0x04,
	WeekDay = 0x08,
	MonthDay = 0x10 | Week | WeekDay,
	Day = 0x20 | WeekDay | MonthDay,
	Hour = 0x40,
	Minute = 0x80
};
Q_DECLARE_FLAGS(Scope, ScopeFlag)

enum WordKey {
	TimePrefix,
	TimeSuffix,
	TimePattern
};

} // break namespace to declare flag operators

Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::Type)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::Scope)

namespace Expressions { //.. and continue it

class REMINDMELIBSHARED_EXPORT SubTerm
{
	Q_DISABLE_COPY(SubTerm)
public:
	inline SubTerm(Type t = InvalidType, Scope s = InvalidScope, bool c = false) :
		type{t},
		scope{s},
		certain{c}
	{}
	virtual ~SubTerm();

	Type type;
	Scope scope;
	bool certain;

	virtual void apply(QDateTime &datetime) = 0;
};

class REMINDMELIBSHARED_EXPORT TimeTerm : public SubTerm
{
public:
	inline TimeTerm(QTime time, bool certain) :
		SubTerm{RelativeTimepoint, Hour | Minute, certain},
		_time{time}
	{}

	static std::pair<QSharedPointer<TimeTerm>, int> parse(const QStringRef &expression);

	void apply(QDateTime &datetime) override;

private:
	QTime _time;

	static QString toRegex(QString pattern);
};



QString trWord(WordKey key, bool escape = true);
QStringList trList(WordKey key, bool escape = true);

}

class REMINDMELIBSHARED_EXPORT EventExpressionParser : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit EventExpressionParser(QObject *parent = nullptr);

	//QSharedPointer<Schedule> parseSchedule(const QString &expression);
	//QDateTime parseSnoozeTime(const QString &expression);

private:
	SyncedSettings *_settings = nullptr;
};

#endif // EVENTEXPRESSIONPARSER_H
