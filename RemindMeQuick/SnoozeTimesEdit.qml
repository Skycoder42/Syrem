import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

Item {
	id: snoozeView

	property var times
	onTimesChanged: {
		if(!inputValue)
			elementList.model = times;
	}

	property var inputValue
	onInputValueChanged:  {
		if(inputValue)
			elementList.model = inputValue.toList();
	}

	implicitHeight: layout.implicitHeight

	ColumnLayout {
		id: layout

		anchors.fill: parent

		ListView {
			id: elementList

			implicitHeight: dummyDelegate.height * (model ? model.length : 1)

			Layout.minimumHeight: dummyDelegate.height
			Layout.preferredHeight: implicitHeight
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true

			delegate: ItemDelegate {
				width: parent.width
				text: modelData
			}

			ItemDelegate {
				id: dummyDelegate
				visible: false
				text: "dummy"
			}
		}

		TextField {
			id: editInput
			Layout.minimumHeight: implicitHeight
			Layout.maximumHeight: implicitHeight
			Layout.fillWidth: true

			AppBarButton {
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: editInput.text != ""

				imageSource: "image://svg/icons/ic_add"
				text: qsTr("Add snooze time")

				onClicked: {
					var nList = elementList.model;
					nList.push(editInput.text);
					elementList.model = nList;
					editInput.text = "";
				}
			}
		}
	}
}
