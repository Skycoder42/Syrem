#include "reminder.h"

#include <QRegularExpression>
#include <QDesktopServices>

using namespace QtDataSync;

class ReminderData : public QSharedData
{
public:
	ReminderData() = default;
	ReminderData(const ReminderData &other) = default;

	QUuid id = QUuid::createUuid();
	quint32 versionCode = 1;
	QString text;
	bool important = false;
	QSharedPointer<Schedule> schedule;
	QDateTime snooze;
	QString expression;
};

Reminder::Reminder() :
	_data(new ReminderData)
{}

Reminder::Reminder(const Reminder &rhs) = default;

Reminder::Reminder(Reminder &&rhs) noexcept = default;

Reminder &Reminder::operator=(const Reminder &rhs) = default;

Reminder &Reminder::operator=(Reminder &&rhs) noexcept = default;

Reminder::~Reminder() = default;

QUuid Reminder::id() const
{
	return _data->id;
}

quint32 Reminder::versionCode() const
{
	return _data->versionCode;
}

QString Reminder::description() const
{
	return _data->text;
}

bool Reminder::isImportant() const
{
	return _data->important;
}

QDateTime Reminder::current() const
{
	if(_data->schedule) {
		if(_data->snooze.isValid())
			return _data->snooze;
		else
			return _data->schedule->current();
	} else
		return {};
}

bool Reminder::isRepeating() const
{
	if(_data->schedule)
		return _data->schedule->isRepeating();
	else
		return false;
}

Reminder::State Reminder::triggerState() const
{
	if(current() <= QDateTime::currentDateTime())
		return Triggered;
	else if(snooze().isValid())
		return Snoozed;
	else if(isRepeating())
		return NormalRepeating;
	else
		return Normal;
}

QSharedPointer<const Schedule> Reminder::schedule() const
{
	return _data->schedule.constCast<const Schedule>();
}

QDateTime Reminder::snooze() const
{
	return _data->snooze;
}

QString Reminder::expression() const
{
	return _data->expression;
}

QList<QUrl> Reminder::extractUrls() const
{
	if(!_urlCache.set) {
		_urlCache.urls.clear();
		QRegularExpression regex {QStringLiteral(R"__((?:\w+):\/\/\S+)__")};
		auto matchIter = regex.globalMatch(_data->text);
		while(matchIter.hasNext()) {
			auto match = matchIter.next();
			QUrl url{match.captured(0)};
			if(url.isValid() && !url.isRelative())
				_urlCache.urls.append(url);
		}
		_urlCache.set = true;
	}

	return _urlCache.urls;
}

QString Reminder::htmlDescription() const
{
	QRegularExpression regex {QStringLiteral(R"__(((?:\w+):\/\/\S+))__")};
	auto text = _data->text;
	return text.toHtmlEscaped().replace(regex, QStringLiteral(R"__(<a href=\"\1\">\1</a>)__"));
}

bool Reminder::hasUrls() const
{
	return !extractUrls().isEmpty();
}

void Reminder::nextSchedule(DataStore *store, const QDateTime &current)
{
	Q_ASSERT_X(_data->schedule, Q_FUNC_INFO, "cannot call next schedule without an assigned schedule");

	QDateTime res;
	do {
		res = _data->schedule->nextSchedule();
	} while(res.isValid() && res <= current);

	_data->snooze = QDateTime();//reset any snoozes
	_data->versionCode++;
	if(res.isValid())
		store->save(*this);
	else
		store->remove<Reminder>(_data->id);
}

void Reminder::performSnooze(DataStore *store, const QDateTime &snooze)
{
	if(_data->schedule && snooze <= _data->schedule->current())
		throw EventExpressionParserException{EventExpressionParser::tr("The snooze time must be in the future of the normal reminder time and not in the past of it.")};
	_data->snooze = snooze;
	_data->versionCode++;
	store->save(*this);
}

void Reminder::openUrls() const
{
	for(const auto &url : extractUrls())
		QDesktopServices::openUrl(url);
}

void Reminder::setId(QUuid id)
{
	_data->id = id;
}

void Reminder::setDescription(QString text)
{
	_data->text = std::move(text);
	_urlCache.urls.clear();
	_urlCache.set = false;
}

void Reminder::setImportant(bool important)
{
	_data->important = important;
}

void Reminder::setSchedule(QSharedPointer<Schedule> schedule)
{
	_data->schedule = std::move(schedule);
}

void Reminder::setExpression(QString expression)
{
	_data->expression = std::move(expression);
}

bool Reminder::operator==(const Reminder &other) const
{
	return _data == other._data || (
		_data->id == other._data->id &&
		_data->versionCode == other._data->versionCode &&
		_data->text == other._data->text &&
		_data->important == other._data->important &&
		_data->schedule == other._data->schedule &&
		_data->snooze == other._data->snooze &&
		_data->expression == other._data->expression);
}

void Reminder::setVersionCode(quint32 versionCode)
{
	_data->versionCode = versionCode;
}

QSharedPointer<Schedule> Reminder::getSchedule() const
{
	return _data->schedule;
}

void Reminder::setSnooze(QDateTime snooze)
{
	_data->snooze = std::move(snooze);
}



uint qHash(const Reminder &reminder, uint seed)
{
	return qHash(reminder._data->id, seed) ^
			qHash(reminder._data->versionCode, seed) ^
			qHash(reminder._data->text, seed) ^
			qHash(reminder._data->important, seed) ^
			qHash(reminder._data->schedule, seed) ^
			qHash(reminder._data->snooze, seed) ^
			qHash(reminder._data->expression, seed);
}
