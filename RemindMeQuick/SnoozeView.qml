import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.remindme 1.0
import de.skycoder42.qtmvvm.quick 1.0
import de.skycoder42.quickextras 2.0

AlertDialog {
	id: snoozeDialog
	title: qsTr("Snooze Reminder")
	property SnoozeControl control: null

	ColumnLayout {
		width: parent.width

		Label {
			Layout.fillWidth: true
			text: {
				if(control) {
					if(control.description === "")
						return qsTr("<i>Loading Reminder, please wait…</i>");
					else
						return qsTr("Choose a snooze time for the reminder:<br/><i>%1</i>").arg(control.description);
				} else
					return "";
			}
		}

		ComboBox {
			id: snoozeBox
			Layout.fillWidth: true
			editable: true

			model: control ? control.snoozeTimes : null

			QtMvvmBinding {
				control: snoozeDialog.control
				controlProperty: "expression"
				view: snoozeBox
				viewProperty: "editText"
				type: QtMvvmBinding.OneWayToControl
			}
		}
	}

	standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

	onAccepted: {
		snoozeDialog.visible = true;
		control.snooze();
	}
}
