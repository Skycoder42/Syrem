import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.syrem 1.0

Page {
	id: createView
	property CreateReminderViewModel viewModel: null
	enabled: !viewModel.blocked

	Connections {
		target: viewModel
		onClose: {
			QuickPresenter.popView()
			// WORKAROUND until fixed in mvvm
			if(coreApp.isCreateOnly())
				Qt.quit();
		}
	}

	header: ContrastToolBar {
		RowLayout {
			anchors.fill: parent
			spacing: 0

			ActionButton {
				icon.name: "arrow-left"
				icon.source: "qrc:/icons/ic_back.svg"
				text: qsTr("Go to reminder overview")
				onClicked: {
					coreApp.resetIsCreateOnly();// WORKAROUND until fixed in mvvm
					viewModel.showMainView();
				}
			}

			ToolBarLabel {
				text: qsTr("Create Reminder")
				Layout.fillWidth: true
				leftPadding: 0
			}

			ActionButton {
				display: ToolButton.TextBesideIcon
				icon.name: "gtk-apply"
				icon.source: "qrc:/icons/ic_check.svg"
				text: qsTr("Create ")
				onClicked: viewModel.create()
			}
		}
	}

	PresenterProgress {}

	ColorHelper {
		id: helper
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Pane {
			Layout.fillWidth: true
			GridLayout {
				width: parent.width
				columns: 2

				DecorLabel {
					Layout.columnSpan: 2
					text: qsTr("Reminder description:")
					edit: textField
				}

				TextField {
					id: textField
					Layout.fillWidth: true
					Layout.columnSpan: 2
					placeholderText: qsTr("Do the thing")
					focus: true
					selectByMouse: true

					MvvmBinding {
						viewModel: createView.viewModel
						viewModelProperty: "text"
						view: textField
						viewProperty: "text"
					}
				}

				DecorLabel {
					Layout.columnSpan: 2
					text: qsTr("When expression:")
					edit: whenField
				}

				TextField {
					id: whenField
					Layout.fillWidth: true
					Layout.columnSpan: 2
					placeholderText: qsTr("in some days at some time")
					selectByMouse: true

					MvvmBinding {
						viewModel: createView.viewModel
						viewModelProperty: "expression"
						view: whenField
						viewProperty: "text"
					}
				}

				Switch {
					id: impSwitch
					Layout.fillWidth: true
					text: qsTr("Important")

					MvvmBinding {
						viewModel: createView.viewModel
						viewModelProperty: "important"
						view: impSwitch
						viewProperty: "checked"
					}
				}

				Button {
					id: helpButton
					checkable: true
					text: qsTr("Help")
				}
			}
		}

		Rectangle {
			visible: helpView.visible
			Layout.fillWidth: true
			Layout.columnSpan: 2
			Layout.minimumHeight: 1.5
			Layout.maximumHeight: 1.5
			color: helper.text
			opacity: 0.5
		}

		ScrollView {
			id: helpView
			visible: helpButton.checked
			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true

			Pane {
				implicitWidth: helpView.width
				width: implicitWidth

				Label {
					id: helpLabel
					anchors.fill: parent
					textFormat: Text.RichText
					wrapMode: Text.Wrap
					text: viewModel.helpText
				}
			}
		}

		Item {
			Layout.preferredHeight: 0
			Layout.fillHeight: true
			Layout.fillWidth: true
		}
	}
}
