import QtQml 2.2
import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import de.skycoder42.QtMvvm.Quick 1.0

SwipeDelegate {
	id: delegate
	width: parent.width
	text: description ? description : ""
	highlighted: important ? important == "true" : false

	signal reminderDeleted
	signal reminderActivated

	readonly property int vCodeDummy: versionCode ? versionCode : 0

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

		TintedIcon { //TODO use normal tint icon
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
					return "qrc:/icons/ic_repeat.svg";
				case 2:
					return "qrc:/icons/ic_snooze.svg";
				case 3:
					return "qrc:/icons/ic_assignment_late.svg";
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
			text: current ? parseDateISOString(current).toLocaleString(Qt.locale(), labelSettings.getFormat()) : ""

			Settings {
				id: labelSettings
				category: "gui"
				property int dateformat: 1

				function getFormat() {
					switch(dateformat) {
					case 0:
						return Locale.LongFormat;
					case 1:
						return Locale.ShortFormat;
					case 2:
						return Locale.NarrowFormat;
					default:
						return Locale.ShortFormat;
					}
				}

				//TODO on settings changed
			}

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

		ActionButton {
			anchors.centerIn: parent
			icon.name: isTriggered ? "gtk-apply" : "user-trash"
			icon.source: isTriggered ? "qrc:/icons/ic_check.svg" : "qrc:/icons/ic_delete_forever.svg"
			text: isTriggered ? qsTr("Complete Reminder") : qsTr("Delete Reminder")

			Material.foreground: "white"

			onClicked: reminderDeleted() //TODO delete or complete!!!
		}
	}
}
