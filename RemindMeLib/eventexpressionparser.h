#ifndef EVENTEXPRESSIONPARSER_H
#define EVENTEXPRESSIONPARSER_H

#include <QObject>
#include <QReadWriteLock>
#include <QUuid>
#include <QVector>
#include <QtMvvmCore/Injection>

#include "remindmelib_global.h"
#include <syncedsettings.h>

class Schedule;
class EventExpressionParser;

namespace Expressions {

class REMINDMELIBSHARED_EXPORT SubTerm : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SubTerm)
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(Type type READ getType WRITE setType)
	Q_PROPERTY(Scope scope READ getScope WRITE setScope)
	Q_PROPERTY(bool certain MEMBER certain)

public:
	enum TypeFlag {
		InvalidType = 0x00,

		Timepoint = 0x01,
		Timespan = 0x02,

		// No flag means relative, unlooped
		FlagAbsolute = 0x10,
		FlagLooped = 0x20,
		FlagLimiter = 0x40,

		AbsoluteTimepoint = Timepoint | FlagAbsolute,
		LoopedTimePoint = Timepoint | FlagLooped,
		LoopedTimeSpan = Timespan | FlagLooped,
		FromSubterm = 0x04 | FlagLimiter,
		UntilSubTerm = 0x08 | FlagLimiter,

		// TODO ... and when in loops, use the timepoints as "from+until" restriction
		// TODO add from/until for limitation of spans
		// TODO add ISO(/RFC) date support
	};
	Q_DECLARE_FLAGS(Type, TypeFlag)
	Q_FLAG(Type)

	enum ScopeFlag {
		InvalidScope = 0x00,

		Year = 0x01,
		Month = 0x02,
		Week = 0x04,
		Day = 0x08,
		WeekDay = Day,
		MonthDay = Week | Day,
		Hour = 0x10,
		Minute = 0x20,
	};
	Q_DECLARE_FLAGS(Scope, ScopeFlag)
	Q_FLAG(Scope)

	SubTerm(Type t = InvalidType, Scope s = InvalidScope, bool c = false);

	Type type;
	Scope scope;
	bool certain;

	virtual void apply(QDateTime &datetime, bool applyRelative) const = 0;

private:
	Type getType() const;
	void setType(Type value);
	Scope getScope() const;
	void setScope(Scope value);
};

class LimiterTerm;

class REMINDMELIBSHARED_EXPORT Term : public QList<QSharedPointer<SubTerm>>
{
public:
	Term() = default;
	Term(const Term &other) = default;
	Term(Term &&other) noexcept = default;
	Term& operator=(const Term &other) = default;
	Term& operator=(Term &&other) noexcept = default;
	inline friend void swap(Term &lhs, Term &rhs) {
		lhs.QList::swap(rhs);
		std::swap(lhs._scope, rhs._scope);
		std::swap(lhs._looped, rhs._looped);
		std::swap(lhs._absolute, rhs._absolute);
	}

	Term(std::initializer_list<QSharedPointer<SubTerm>> args);

	SubTerm::Scope scope() const;
	bool isLooped() const;
	bool isAbsolute() const;
	bool hasTimeScope() const;

	QDateTime apply(QDateTime datetime) const;

private:
	friend class ::EventExpressionParser;
	void finalize();

	SubTerm::Scope _scope = SubTerm::InvalidScope;
	bool _looped = false;
	bool _absolute = false;
};

using TermSelection = QList<Term>;
using MultiTerm = QVector<TermSelection>;

}

class REMINDMELIBSHARED_EXPORT EventExpressionParser : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit EventExpressionParser(QObject *parent = nullptr);

	Expressions::MultiTerm parseMultiExpression(const QString &expression);
	Expressions::TermSelection parseExpression(const QString &expression);

	QSharedPointer<Schedule> createSchedule(const Expressions::Term &term);
	QDateTime evaluteTerm(const Expressions::Term &term, const QDateTime &reference = QDateTime::currentDateTime());

Q_SIGNALS:
	void termCompleted(QUuid termId, int termIndex, const Expressions::Term &term);
	void operationCompleted(QUuid doneId);

private:
	SyncedSettings *_settings = nullptr;

	QReadWriteLock _taskLocker;
	QHash<QUuid, QAtomicInt> _taskCounter;

	Expressions::MultiTerm parseExpressionImpl(const QString &expression, bool allowMulti);

	// direct invokations
	void parseTerm(QUuid id, const QStringRef &expression, const Expressions::Term &term, int termIndex, const Expressions::Term &rootTerm);
	bool validatePartialTerm(const Expressions::Term &term);
	bool validateFullTerm(Expressions::Term &term, Expressions::Term &rootTerm);
	// async invokations
	void parseMultiTerm(QUuid id, const QString *expression, Expressions::MultiTerm *terms);
	template <typename TSubTerm>
	void parseSubTerm(QUuid id, const QStringRef &expression, Expressions::Term term, int termIndex, Expressions::Term rootTerm);

	void addTasks(QUuid id, int count);
	void completeTask(QUuid id);
};

template <>
REMINDMELIBSHARED_EXPORT void EventExpressionParser::parseSubTerm<Expressions::LimiterTerm>(QUuid id, const QStringRef &expression, Expressions::Term term, int termIndex, Expressions::Term rootTerm);

Q_DECLARE_METATYPE(Expressions::Term)
Q_DECLARE_METATYPE(Expressions::TermSelection)
Q_DECLARE_METATYPE(Expressions::MultiTerm)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::SubTerm::Type)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::SubTerm::Scope)

#endif // EVENTEXPRESSIONPARSER_H
