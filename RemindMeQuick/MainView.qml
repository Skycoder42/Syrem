import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0
import de.skycoder42.qtmvvm.quick 1.0
import de.skycoder42.remindme 1.0
import ".."

Page {
	id: mainView
	property MainControl control: null

	header: ActionBar {
		id: toolbar
		title: qsTr("Manager Reminders")
		showMenuButton: false

		moreMenu: Menu {
			id: moreMenu

			MenuItem {
				id: settings
				text: qsTr("Settings")
				onClicked: control.showSettings()
			}

			MenuItem {
				id: sync
				text: qsTr("Synchronization")
				onClicked: control.showSync()
			}

			MenuSeparator {}

			MenuItem {
				id: about
				text: qsTr("About")
			}
		}
	}

	PresenterProgress {}

	ListView {
		anchors.fill: parent

		SortFilterProxyModel {
			id: sortModel
			sourceModel: control ? control.reminderModel : null
			sortRoleName: control ? "id" : ""
		}

		model: sortModel

		ScrollBar.vertical: ScrollBar {}

		delegate: ReminderDelegate {
			onReminderDeleted: control.removeReminder(id)
		}
	}

	FloatingActionButton {
		id: addButton

		anchors.right: parent.right
		anchors.rightMargin: 10
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10

		imageSource: "image://svg/icons/ic_add"
		text: qsTr("Add Reminder")

		onClicked: control.addReminder()
	}
}
