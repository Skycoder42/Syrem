import QtQml 2.2
import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.remindme 1.0

SwipeDelegate {
	id: delegate
	width: parent.width
	text: description
	highlighted: important ? important == "true" : false

	signal reminderCompleted
	signal reminderDeleted
	signal reminderActivated

	function redColor() {
		if(QuickPresenter.currentStyle === "Material")
			return Material.color(Material.Red);
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.color(Universal.Red);
		else
			return "#FF0000";
	}

	function accentColor() {
		if(QuickPresenter.currentStyle === "Material")
			return Material.accent;
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.accent;
		else
			return "#00FFFF";
	}

	contentItem: RowLayout {
		Material.foreground: delegate.highlighted ? Material.accent : Material.foreground

		Label {
			id: titleLabel
			elide: Label.ElideRight
			horizontalAlignment: Qt.AlignLeft
			verticalAlignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.fillHeight: true
			font.bold: delegate.highlighted
			text: delegate.text
		}

		Displace {
			source: imgBtn
			Layout.preferredWidth: imgBtn.width
			Layout.preferredHeight: imgBtn.height

			ActionButton {
				id: imgBtn
				visible: false
				down: false
				icon.name: {
					switch(Number(triggerState)) {
					case 0:
						return "";
					case 1:
						return "media-playlist-repeat";
					case 2:
						return "alarm-symbolic";
					case 3:
						return "view-calendar-upcoming-events";
					default:
						return "";
					}
				}
				icon.source: {
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
		}

		Label {
			id: whenLabel
			Layout.fillHeight: true
			horizontalAlignment: Qt.AlignRight
			verticalAlignment: Qt.AlignVCenter
			text: current ? parseDateISOString(current).toLocaleString(Qt.locale(), SyncedSettings.gui.dateformat) : ""

			function parseDateISOString(s) {
				var b = s.split(/\D/);
				return new Date(b[0], b[1]-1, b[2], b[3], b[4], b[5]);
			}
		}
	}

	onClicked: reminderActivated()

	swipe.right: Rectangle {
		readonly property bool isTriggered: triggerState == Reminder.Triggered || triggerState == Reminder.Snoozed
		anchors.fill: parent
		color: isTriggered ? accentColor() : redColor()

		MouseArea {
			anchors.fill: parent

			ActionButton {
				anchors.centerIn: parent
				icon.name: isTriggered ? "gtk-apply" : "user-trash"
				icon.source: isTriggered ? "qrc:/icons/ic_check.svg" : "qrc:/icons/ic_delete_forever.svg"
				text: isTriggered ? qsTr("Complete Reminder") : qsTr("Delete Reminder")

				Material.foreground: "white"
				Universal.foreground: "white"

				onClicked: {
					delegate.swipe.close();
					isTriggered ? reminderCompleted() : reminderDeleted()
				}
			}
		}
	}
}
