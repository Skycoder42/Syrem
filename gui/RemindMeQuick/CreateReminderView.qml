import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.remindme 1.0

AlertDialog {
	id: createDialog
	title: qsTr("Create Reminder")
	property CreateReminderViewModel viewModel: null
	enabled: !viewModel.blocked

	Connections {
		target: viewModel
		onClose: {
			createDialog.close()
			if(coreApp.isCreateOnly())
				Qt.quit();
		}
	}

	ColumnLayout {
		width: parent.width

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

		Switch {
			id: impSwitch
			Layout.fillWidth: true
			text: qsTr("Important")

			MvvmBinding {
				viewModel: createDialog.viewModel
				viewModelProperty: "important"
				view: impSwitch
				viewProperty: "checked"
			}
		}
	}

	footer: Item {
		implicitWidth: _btnBox.implicitWidth
		implicitHeight: _btnBox.implicitHeight

		DialogButtonBox {
			id: _btnBox
			anchors.fill: parent

			standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

			onAccepted: viewModel.create()
			onRejected: {
				createDialog.reject();
				if(coreApp.isCreateOnly())
					Qt.quit();
			}
		}
	}
}
