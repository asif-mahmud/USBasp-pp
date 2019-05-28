import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

CustomGroupBox {
    id: mcuSelector
    title: qsTr("step 1")
    property var selectedMcu: Object.create({
                                                id: "none",
                                                description:"none",
                                                signature: "none",
                                                flashSize: 0,
                                                eepromSize: 0
                                            })
    property string avrdudePath: qsTr("none")
    property var partList

    function convertSizeString(size) {
        var szNum = Number.parseInt(size)
        if(szNum > 1024) {
            return (szNum/1024) + " kB"
        }
        return szNum + " bytes"
    }

    function setInitialPartIndex(ix) {
        console.log("Initial part index: " + ix)
        mcuSelectorCombo.currentIndex = ix
    }

    onSelectedMcuChanged: {
        if(mcuSelector.selectedMcu.description !== "none") {
            mcuName.color = mcuSelector.darkTheme ? "cyan" : "blue"
            mcuName.font.underline = true
            mcuNameMouseArea.cursorShape = Qt.PointingHandCursor
        }
    }

    onDarkThemeChanged: {
        if(mcuSelector.selectedMcu.description !== "none") {
            mcuName.color = mcuSelector.darkTheme ? "cyan" : "blue"
        }
    }

    ColumnLayout {
        id: contents
        width: mcuSelector.availableWidth
        spacing: 3

        Label {
            text: qsTr("Select Microcontroller")
            Layout.fillWidth: true
        }

        ComboBox {
            id: mcuSelectorCombo
            Layout.fillWidth: true
            editable: true
            textRole: "description"
            model: mcuSelector.partList

            onCurrentIndexChanged: {
                mcuSelector.selectedMcu = mcuSelector.partList[mcuSelectorCombo.currentIndex]
            }
        }

        Label {
            text: ((mcuSelector.partList && mcuSelector.partList.length > 0) ? qsTr("Total ") + mcuSelector.partList.length + qsTr(" parts supported") : "No part found")
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: qsTr("ID: ") + mcuSelector.selectedMcu.id
            Layout.fillWidth: true
        }

        RowLayout {
            Label {
                text: qsTr("Name: ")
            }

            Label {
                id: mcuName
                property string curLink: qsTr("https://www.alldatasheet.com/view.jsp?Searchword=") +
                                         mcuSelector.selectedMcu.description
                text: mcuSelector.selectedMcu.description

                MouseArea {
                    id: mcuNameMouseArea
                    anchors.fill: parent
                    onClicked: {
                        if(mcuSelector.selectedMcu.description !== "none") {
                            Qt.openUrlExternally(parent.curLink)
                        }
                    }
                }
            }
        }

        Label {
            text: qsTr("Signature: ") + mcuSelector.selectedMcu.signature
            Layout.fillWidth: true
        }
        Label {
            text: qsTr("Flash Size: ") + convertSizeString(mcuSelector.selectedMcu.flashSize)
            Layout.fillWidth: true
        }
        Label {
            text: qsTr("ROM Size: ") + convertSizeString(mcuSelector.selectedMcu.eepromSize)
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Avrdude: ") + (mcuSelector.avrdudePath === "none" ? qsTr("not found") : qsTr("found as ") + mcuSelector.avrdudePath)
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ProgressBar {
            id: progressBar
            from: 0
            to: 100
            value: 50
            visible: false
            Layout.topMargin: 20
            Layout.fillWidth: true

            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 12
                color: "#e6e6e6"
                radius: 0
            }

            contentItem: Item {
                implicitWidth: 200
                implicitHeight: 12

                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: 0
                    color: "#17a81a"
                }
            }
        }
    }
}
