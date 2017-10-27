#include "androidnotifier.h"

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier()
{}

void AndroidNotifier::beginSetup()
{
}

void AndroidNotifier::endSetup()
{
}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
}
