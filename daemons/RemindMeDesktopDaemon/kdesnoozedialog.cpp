#include "kdesnoozedialog.h"
#include <dialogmaster.h>
#include <localsettings.h>

KdeSnoozeDialog::KdeSnoozeDialog(SyncedSettings *settings, DateParser *parser, const QString &description, QWidget *parent) :
	QInputDialog(parent),
	_parser(parser)
{
	setWindowTitle(tr("Snooze Reminder"));
	setLabelText(tr("Choose a snooze time for the reminder:<br/><i>%1</i>")
				 .arg(description));
	setInputMode(QInputDialog::TextInput);
	setComboBoxItems(settings->scheduler.snoozetimes);
	setComboBoxEditable(true);
	DialogMaster::masterDialog(this);

	if(LocalSettings::instance()->gui.snoozedialog.size.isSet())
		resize(LocalSettings::instance()->gui.snoozedialog.size);
}

void KdeSnoozeDialog::accept()
{
	try {
		auto nextTime = _parser->snoozeParse(textValue());
		emit timeSelected(nextTime);
		QDialog::accept();
	} catch (DateParserException &e) {
		DialogMaster::critical(this, e.qWhat(), tr("Snoozing failed!"));
	}
}
