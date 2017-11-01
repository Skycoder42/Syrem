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
			text: control ? control.description : ""
		}

		ComboBox {
			id: snoozeBox
			Layout.fillWidth: true
			editable: true
			enabled: control && control.loaded

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

	standardButtons: control && control.loaded ? (DialogButtonBox.Ok | DialogButtonBox.Cancel) : DialogButtonBox.Cancel

	onAccepted: {
		snoozeDialog.visible = true;
		control.snooze();
	}
}
