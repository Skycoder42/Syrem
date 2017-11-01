import QtQml 2.2
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

SwipeDelegate {
	id: delegate
	width: parent.width
	text: description ? description : ""
	highlighted: important ? important == "true" : false

	signal reminderDeleted
	signal reminderActivated

	readonly property int vCodeDummy: versionCode ? versionCode : 0

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

		function accentColor() {
			if(style.isMaterial)
				return style.accent;
			else if(style.isUniversal)
				return style.accent;
			else
				return "#00FFFF";
		}
	}

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
				switch(Number(triggerState)) {
				case 0:
					return "";
				case 1:
					return "image://svg/icons/ic_repeat";
				case 2:
					return "image://svg/icons/ic_snooze";
				case 3:
					return "image://svg/icons/ic_assignment_late";
				default:
					return "";
				}
			}
		}

		Label {
			id: whenLabel
			Layout.fillHeight: true
			horizontalAlignment: Qt.AlignRight
			verticalAlignment: Qt.AlignVCenter
			color: style.highlight(delegate.highlighted)
			text: current ? new parseDateISOString(current).toLocaleString(Qt.locale(), Locale.ShortFormat) : "" //TODO settings format

			function parseDateISOString(s) {
				var b = s.split(/\D/);
				return new Date(b[0], b[1]-1, b[2], b[3], b[4], b[5]);
			}
		}
	}

	onClicked: {
		if(triggerState == 3)
			reminderActivated()
	}

	swipe.right: Rectangle {
		readonly property bool isTriggered: triggerState == 3 || triggerState == 2

		width: parent.width
		height: parent.height
		anchors.right: parent.right
		color: isTriggered ? style.accentColor() : style.redColor()
		AppBarButton {
			anchors.fill: parent
			imageSource: isTriggered ? "image://svg/icons/ic_check" : "image://svg/icons/ic_delete_forever"
			text: isTriggered ? qsTr("Complete Reminder") : qsTr("Delete Reminder")

			Material.foreground: "white"

			onClicked: reminderDeleted()
		}
	}
}
