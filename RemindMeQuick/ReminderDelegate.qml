import QtQml 2.2
import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

SwipeDelegate {
	id: delegate
	width: parent.width
	text: description ? description : ""
	highlighted: important ? important == "true" : false

	CommonStyle {
		id: style

		function highlight(high) {
			if(style.isMaterial)
				return high ? style.accent : style.foreground;
			else
				return style.foreground;
		}

		function redColor() {
			if(style.isMaterial)
				return Material.color(Material.Red);
			else if(style.isUniversal)
				return Universal.color(Universal.Red);
			else
				return "#FF0000";
		}
	}

	signal reminderDeleted

	contentItem: RowLayout {
		Label {
			id: titleLabel
			elide: Label.ElideRight
			horizontalAlignment: Qt.AlignLeft
			verticalAlignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.fillHeight: true
			font.bold: delegate.highlighted
			color: style.highlight(delegate.highlighted)
			text: delegate.text
		}

		TintIcon {
			id: stateImage

			Layout.minimumWidth: 42
			Layout.maximumWidth: 42
			Layout.minimumHeight: 42
			Layout.maximumHeight: 42

			tintColor: highlighted ? style.accent : style.foreground

			source: {
				if(snooze)
					return "image://svg/icons/ic_snooze";
				else if(repeating == "true")
					return "image://svg/icons/ic_repeat";
				else
					return "";
			}
		}

		Label {
			id: whenLabel
			Layout.fillHeight: true
			horizontalAlignment: Qt.AlignRight
			verticalAlignment: Qt.AlignVCenter
			color: style.highlight(delegate.highlighted)
			text: current ? new Date(current).toLocaleString(Qt.locale(), Locale.ShortFormat) : "" //TODO settings format
		}
	}

	swipe.right: Rectangle {
		width: parent.height
		height: parent.height
		anchors.right: parent.right
		color: style.redColor()
		AppBarButton {
			size: parent.height
			imageSource: "image://svg/icons/ic_delete_forever"
			text: qsTr("Delete Reminder")

			Material.foreground: "white"

			onClicked: reminderDeleted()
		}
	}
}
