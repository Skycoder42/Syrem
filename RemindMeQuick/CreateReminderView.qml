import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.remindme 1.0
import de.skycoder42.qtmvvm.quick 1.0
import de.skycoder42.quickextras 2.0

AlertDialog {
	id: createDialog
	title: qsTr("Create Reminder")
	property CreateReminderControl control: null

	Connections {
		target: control
		onCreateCompleted: {
			if(success)
				close();
		}
	}

	GridLayout {
		columns: 2
		width: parent.width

		Label {
			text: qsTr("Text:")
		}

		TextField {
			id: textField
			Layout.fillWidth: true
			placeholderText: qsTr("Reminder description")
			focus: true
			selectByMouse: true

			QtMvvmBinding {
				control: createDialog.control
				controlProperty: "text"
				view: textField
				viewProperty: "text"
			}
		}

		Label {
			text: qsTr("When:")
		}

		TextField {
			id: whenField
			Layout.fillWidth: true
			placeholderText: qsTr("When expression")
			selectByMouse: true

			QtMvvmBinding {
				control: createDialog.control
				controlProperty: "expression"
				view: whenField
				viewProperty: "text"
			}
		}

		Label {
			text: qsTr("Important:")
		}

		Switch {
			id: impSwitch

			QtMvvmBinding {
				control: createDialog.control
				controlProperty: "important"
				view: impSwitch
				viewProperty: "checked"
			}
		}
	}

	standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

	onAccepted: {
		createDialog.visible = true;
		control.create();
	}
}
