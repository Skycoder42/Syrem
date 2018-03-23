import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.remindme 1.0

AlertDialog {
	id: createDialog
	title: qsTr("Create Reminder")
	property CreateReminderViewModel viewModel: null

	Connections {
		target: viewModel
		onClose: {
			createDialog.close()
			if(coreApp.isCreateOnly())
				Qt.quit();
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

			MvvmBinding {
				viewModel: createDialog.viewModel
				viewModelProperty: "text"
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

			MvvmBinding {
				viewModel: createDialog.viewModel
				viewModelProperty: "expression"
				view: whenField
				viewProperty: "text"
			}
		}

		Label {
			text: qsTr("Important:")
		}

		Switch {
			id: impSwitch

			MvvmBinding {
				viewModel: createDialog.viewModel
				viewModelProperty: "important"
				view: impSwitch
				viewProperty: "checked"
			}
		}
	}

	standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

	onAccepted: {
		if(viewModel.create())
			createDialog.enabled = false;
		createDialog.visible = true;
	}

	onRejected: {
		if(coreApp.isCreateOnly())
			Qt.quit();
	}
}
