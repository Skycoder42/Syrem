import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.remindme 1.0

AlertDialog {
	title: qsTr("Select interpretation")
	property TermSelectionViewModel viewModel: null

	ColumnLayout {
		anchors.fill: parent

		TabBar {
			id: tabs
			Layout.fillWidth: true
			currentIndex: view.currentIndex
			visible: viewModel.multi

			Repeater {
				model: viewModel.varTerms

				delegate: TabButton {
					text: qsTr("SubTerm %L1").arg(modelData)
				}
			}
		}

		SwipeView {
			id: view
			currentIndex: tabs.currentIndex
			Layout.fillWidth: true
			Layout.fillHeight: true
			implicitHeight: {
				var max = 0;
				for(var i = 0; i < count; ++i)
					max = Math.max(max, itemAt(i).implicitHeight);
				return max;
			}
			clip: true

			Repeater {
				model: viewModel.varTerms

				ScrollView {
					id: scrollView
					implicitHeight: dummyDelegate.height * listView.model.length * 1.5
					clip: true

					readonly property int termIndex: modelData
					readonly property var sizes: new Array(listView.model.length)

					ListView {
						id: listView
						model: viewModel.describeChoices(modelData)
						delegate: RadioDelegate {
							id: delegate
							readonly property int choiceIndex: index
							width: scrollView.width
							text: modelData
							onHeightChanged: scrollView.sizes[index] = height

							contentItem: Label {
								text: delegate.text
								wrapMode: Text.WordWrap
							}

							ToolTip.visible: pressed
							ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
							ToolTip.text: modelData

							onCheckedChanged: {
								if(checked)
									viewModel.selectChoice(termIndex, choiceIndex);
							}
						}
					}
				}
			}
		}

		RadioDelegate {
			id: dummyDelegate
			visible: false
		}
	}

	footer: DialogButtonBox {
		id: footer

		standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
		onAccepted: viewModel.complete()

		Component.onCompleted: {
			var btn = footer.standardButton(DialogButtonBox.Ok);
			btn.enabled = Qt.binding(function(){ return viewModel.allHandled; });
		}
	}
}
