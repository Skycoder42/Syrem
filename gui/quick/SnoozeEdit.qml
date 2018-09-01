import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

RowLayout {
	id: snoozeEdit

	property bool edit: false
	property alias text: editInput.text

	signal editDone()
	signal removed()
	signal focusLost()

	function activate() {
		editInput.selectAll();
		editInput.forceActiveFocus();
	}

	TextField {
		id: editInput
		Layout.fillWidth: true

		placeholderText: edit ? qsTr("Edit snooze time…") : qsTr("Add new snooze time…")
		selectByMouse: true

		onFocusChanged: {
			if(!focus)
				focusLost();
		}
	}

	ActionButton {
		enabled: editInput.text != ""

		icon.name: edit ? "gtk-apply" : "gtk-add"
		icon.source: edit ? "qrc:/icons/ic_check.svg" : "qrc:/icons/ic_add.svg"
		text: edit ? qsTr("Update snooze time…") : qsTr("Add snooze time")

		onClicked: editInput.text == "" ? removed() : editDone()
	}
}
