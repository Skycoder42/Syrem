import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

TextField {
	id: editInput

	property bool edit: false

	signal editDone()
	signal removed()

	placeholderText: edit ? qsTr("Edit snooze time…") : qsTr("Add new snooze time…")

	ActionButton {
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		visible: editInput.text != ""

		icon.name: edit ? "gtk-apply" : "gtk-add"
		icon.source: edit ? "qrc:/icons/ic_check.svg" : "qrc:/icons/ic_add.svg"
		text: edit ? qsTr("Update snooze time…") : qsTr("Add snooze time")

		onClicked: editInput.text == "" ? removed() : editDone()
	}
}
