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
	enabled: !viewModel.blocked

	Connections {
		target: viewModel
		onClose: snoozeDialog.close()
	}

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


	footer: RowLayout {
		implicitHeight: _btnBox.implicitHeight

		Button {
			visible: viewModel.reminder.hasUrls
			flat: true
			highlighted: true
			text: qsTr("Open URLs")
			Layout.leftMargin: _btnBox.leftPadding
			onClicked: {
				viewModel.reminder.openUrls();
				snoozeDialog.reject();
			}
		}

		DialogButtonBox {
			id: _btnBox
			Layout.fillWidth: true

			standardButtons: viewModel.valid ? (DialogButtonBox.Ok | DialogButtonBox.Cancel) : DialogButtonBox.Cancel

			onAccepted: viewModel.snooze();
			onRejected: snoozeDialog.reject()
		}
	}
}
