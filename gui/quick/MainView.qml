import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.syrem 1.0
import "../../qml"

Page {
	id: mainView
	property MainViewModel viewModel: null
	readonly property bool presentAsRoot: true

	header: ContrastToolBar {
		RowLayout {
			anchors.fill: parent
			spacing: 0

			ToolBarLabel {
				text: qsTr("Reminders")
				Layout.fillWidth: true
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

	ScrollView {
		id: scrollView
		anchors.fill: parent

		ListView {
			model: viewModel.sortedModel

			delegate: ReminderDelegate {
				width: scrollView.width
				onReminderActivated: viewModel.snoozeReminder(id)
				onReminderCompleted: viewModel.completeReminder(id)
				onReminderDeleted: viewModel.deleteReminder(id)
				onReminderOpenUrl: viewModel.openReminderUrls(id);
			}
		}
	}

	RoundActionButton {
		id: addButton

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 16

		icon.name: "gtk-add"
		icon.source: "qrc:/icons/ic_add.svg"
		text: qsTr("Add Reminder")

		onClicked: viewModel.addReminder()
	}
}
