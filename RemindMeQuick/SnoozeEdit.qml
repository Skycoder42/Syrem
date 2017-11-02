import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

TextField {
	id: editInput

	property bool edit: false

	signal editDone()

	placeholderText: edit ? qsTr("Edit snooze time…") : qsTr("Add new snooze time…")

	AppBarButton {
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		visible: editInput.text != ""

		imageSource: edit ? "image://svg/icons/ic_check" : "image://svg/icons/ic_add"
		text: edit ? qsTr("Update snooze time…") : qsTr("Add snooze time")

		onClicked: editDone()
	}
}
