import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

AlertDialog {
	id: aboutDialog

	property string text
	property url websiteUrl: ""
	property string licenseName: ""
	property url licenseUrl: ""
	property string companyName: Qt.application.organization
	property bool addQtInfo: true
	property string extraVersionInfo: ""

	title: qsTr("About %1\nVersion %2")
		.arg(Qt.application.displayName)
		.arg(Qt.application.version)

	contentItem: ColumnLayout {
		Label {
			id: aboutContent
			wrapMode: Label.Wrap
			Layout.fillWidth: true
			onLinkActivated: Qt.openUrlExternally(link)

			text: {
				var aboutText = "<p>" + aboutDialog.text + "</p><br/>";
				if(addQtInfo || extraVersionInfo !== "") {
					aboutText += "<p>";
					if(extraVersionInfo !== "")
						aboutText += extraVersionInfo + "<br/>";
					if(addQtInfo)
						aboutText += qsTr("Qt-Version: <a href=\"https://www.qt.io/\">%1</a>").arg(qtVersion);
					aboutText += "</p><br/>";
				}
				aboutText += "<p>" + qsTr("Developed by: %1").arg(companyName);
				if(websiteUrl != "")
					aboutText += "<br/>" + qsTr("Project Website: <a href=\"%1\">%2</a>").arg(websiteUrl).arg(websiteUrl.toString());
				if(licenseName !== "") {
					if(licenseUrl != "")
						aboutText += "<br/>" + qsTr("License: <a href=\"%1\">%2</a>").arg(licenseUrl).arg(licenseName);
					else
						aboutText += "<br/>" + qsTr("License: %1").arg(licenseName);
				}
				aboutText += "</p>";
				return aboutText;
			}
		}
	}

	standardButtons: DialogButtonBox.Ok
}
