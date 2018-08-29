import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.remindme 1.0

AlertDialog {
	id: snoozeDialog
	title: qsTr("Snooze Reminder")
	property SnoozeViewModel viewModel: null

	ColumnLayout {
		width: parent.width

		Label {
			Layout.fillWidth: true
			text: viewModel.description
		}

		ComboBox {
			id: snoozeBox
			Layout.fillWidth: true
			editable: true
			enabled: viewModel.valid

			model: viewModel.snoozeTimes

			MvvmBinding {
				viewModel: snoozeDialog.viewModel
				viewModelProperty: "expression"
				view: snoozeBox
				viewProperty: "editText"
				type: MvvmBinding.OneWayToViewModel
			}

			Component.onCompleted: {
				selectAll();
				snoozeBox.forceActiveFocus();
			}

			onAccepted: snoozeDialog.accept()
		}
	}


	footer: Item {
		implicitWidth: _btnBox.implicitWidth
		implicitHeight: _btnBox.implicitHeight

		DialogButtonBox {
			id: _btnBox
			anchors.fill: parent

			standardButtons: viewModel.valid ? (DialogButtonBox.Ok | DialogButtonBox.Cancel) : DialogButtonBox.Cancel

			onAccepted: {
				if(viewModel.snooze())
					snoozeDialog.accept();
			}
			onRejected: snoozeDialog.reject()
		}
	}
}
