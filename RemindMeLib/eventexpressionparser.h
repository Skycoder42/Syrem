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

		Minute = 0x01,
		Hour = 0x02,
		Day = 0x04,
		WeekDay = Day,
		Week = 0x08,
		MonthDay = Week | Day,
		Month = 0x10,
		Year = 0x20,
	};
	Q_DECLARE_FLAGS(Scope, ScopeFlag)
	Q_FLAG(Scope)

	SubTerm(Type type = InvalidType, Scope scope = InvalidScope);

	Type type;
	Scope scope;

	virtual void apply(QDateTime &datetime, bool applyRelative) const = 0;
	virtual void fixup(QDateTime &datetime) const;

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
	Term(const QList<QSharedPointer<SubTerm>> &list);

	SubTerm::Scope scope() const;
	bool isLooped() const;
	bool isAbsolute() const;
	bool hasTimeScope() const;

	QDateTime apply(const QDateTime &datetime, bool applyRelative = true) const;
	std::tuple<Term, Term, Term, Term> splitLoop() const; //(loop, fence, from, until)

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
	enum ErrorType {
		NoError = 0,

		ParserError = 1,
		DuplicateScopeError,
		DuplicateLoopError,
		DuplicateSpanError,
		DuplicateFromLimiterError,
		DuplicateUntilLimiterError,
		UnexpectedLimiterError,
		UnexpectedAbsoluteSubTermError, // currently impossible to test, as only year scopes are absolute
		SpanAfterTimepointError,
		LoopAsLimiterError,
		LimiterSmallerThanFenceError,

		TermIsLoopError,
		EvaluatesToPastError,
		UntilIsSmallerThenPastError,
		InitialLoopInvalidError,

		UnknownError = -1
	};
	Q_ENUM(ErrorType)

	struct ErrorInfo {
		enum : quint32 {
			NoneLevel = 0x00,
			ParsingLevel = 0x01,
			SubTermLevel = 0x02,
			TermLevel = 0x03
		} level = NoneLevel;
		int depth = 0;

		ErrorType type = NoError;
		int subTermBegin = -1;

		quint64 calcSignificance() const;
	};

	Q_INVOKABLE explicit EventExpressionParser(QObject *parent = nullptr);

	Expressions::MultiTerm parseMultiExpression(const QString &expression);
	Expressions::TermSelection parseExpression(const QString &expression);

	QSharedPointer<Schedule> createSchedule(const Expressions::Term &term, const QDateTime &reference = QDateTime::currentDateTime());
	QDateTime evaluteTerm(const Expressions::Term &term, const QDateTime &reference = QDateTime::currentDateTime());

Q_SIGNALS:
	void termCompleted(QUuid termId, int termIndex, const Expressions::Term &term);
	void errorOccured(QUuid termId, quint64 significance, const ErrorInfo &info);
	void operationCompleted(QUuid doneId);

private:
	friend class EventExpressionParserException;

	SyncedSettings *_settings = nullptr;

	QReadWriteLock _taskLocker;
	QHash<QUuid, std::pair<QAtomicInt, QAtomicInteger<quint64>>> _taskCounter;

	Expressions::MultiTerm parseExpressionImpl(const QString &expression, bool allowMulti);

	// direct invokations
	void parseTerm(QUuid id, const QStringRef &expression, const Expressions::Term &term, int termIndex, const Expressions::Term &rootTerm, int depth);
	void validatePartialTerm(const Expressions::Term &term, int depth);
	void validateFullTerm(Expressions::Term &term, Expressions::Term &rootTerm, int depth);
	// async invokations
	void parseMultiTerm(QUuid id, const QString *expression, Expressions::MultiTerm *terms);
	struct TermParams {
		QUuid id;
		QStringRef expression;
		Expressions::Term term;
		int termIndex;
		Expressions::Term rootTerm;
		int depth;
	};
	template <typename TSubTerm>
	void parseSubTerm(TermParams params);
	template <typename TSubTerm>
	void parseSubTermImpl(QUuid id, const QStringRef &expression, Expressions::Term term, int termIndex, Expressions::Term rootTerm, int depth);

	void addTasks(QUuid id, int count);
	void reportError(QUuid id, const ErrorInfo &info, bool autoComplete);
	void completeTask(QUuid id);
	void completeTask(QUuid id, QReadLocker &);

	static QString createErrorMessage(ErrorType type, int depthEnd = 0, const QStringRef &subTerm = {});
};

class REMINDMELIBSHARED_EXPORT EventExpressionParserException : public QException
{
public:
	EventExpressionParserException(EventExpressionParser::ErrorType type, int depthEnd = 0, const QStringRef &subTerm = {});

	QString message() const;
	EventExpressionParser::ErrorType type() const;

	QString qWhat() const;
	const char *what() const noexcept override;
	void raise() const override;
	QException *clone() const override;

protected:
	EventExpressionParserException(const EventExpressionParserException * const other);

	const EventExpressionParser::ErrorType _type;
	const QString _message;
	const QByteArray _what;
};

// stuff

template <>
REMINDMELIBSHARED_EXPORT void EventExpressionParser::parseSubTermImpl<Expressions::LimiterTerm>(QUuid id, const QStringRef &expression, Expressions::Term term, int termIndex, Expressions::Term rootTerm, int depth);

Q_DECLARE_METATYPE(Expressions::Term)
Q_DECLARE_METATYPE(Expressions::TermSelection)
Q_DECLARE_METATYPE(Expressions::MultiTerm)
Q_DECLARE_METATYPE(EventExpressionParser::ErrorInfo)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::SubTerm::Type)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::SubTerm::Scope)

#endif // EVENTEXPRESSIONPARSER_H
