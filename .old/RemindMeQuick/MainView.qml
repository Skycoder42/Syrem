import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.remindme 1.0
import "../../qml"

Page {
	id: mainView
	property MainViewModel viewModel: null

	header: ContrastToolBar {
		id: toolbar

		RowLayout {
			id: toolLayout
			anchors.fill: parent
			spacing: 0

			ToolBarLabel {
				id: titleLabel
				Layout.fillWidth: true
				text: qsTr("Manager Reminders")
			}

			MenuButton {
				MenuItem {
					id: settings
					text: qsTr("Settings")
					onClicked: viewModel.showSettings()
				}

				MenuItem {
					id: sync
					text: qsTr("Synchronization")
					onClicked: viewModel.showSync()
				}

				MenuSeparator {}

				MenuItem {
					id: about
					text: qsTr("About")
					onClicked: viewModel.showAbout()
				}
			}
		}
	}

	PresenterProgress {}

	ListView {
		anchors.fill: parent

		model: viewModel.reminderModel //TODO sort in viewmodel

		ScrollBar.vertical: ScrollBar {}

		delegate: ReminderDelegate {
			onReminderActivated: viewModel.snoozeReminder(id, versionCode)
			onReminderDeleted: viewModel.removeReminder(id)
		}
	}

	RoundActionButton {
		id: addButton

		Material.foreground: "white"

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 16

		icon.name: "gtk-add"
		icon.source: "qrc:/icons/ic_add.svg"
		text: qsTr("Add Reminder")

		onClicked: viewModel.addReminder()
	}
}
