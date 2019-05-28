import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

CustomGroupBox {
    id: inputBox
    title: "step 2"

    ColumnLayout {
        id: contents
        width: inputBox.availableWidth

        Button {
            Layout.fillWidth: true
            text: "Open HEX file"
        }

        RowLayout {
            Layout.fillWidth: true
            Label {
                Layout.preferredWidth: inputBox.width/3
                text: qsTr("Low Fuse :")
            }

            TextField {
                id: lowFuseInput
                Layout.fillWidth: true
                placeholderText: qsTr("Low Fuse")
            }
        }

        RowLayout {
            Label {
                Layout.preferredWidth: inputBox.width/3
                text: qsTr("High Fuse :")
            }

            TextField {
                id: highFuseInput
                Layout.fillWidth: true
                placeholderText: qsTr("High Fuse")
            }
        }

        RowLayout {
            Label {
                Layout.preferredWidth: inputBox.width/3
                text: qsTr("Ext. Fuse :")
            }

            TextField {
                id: extFuseInput
                Layout.fillWidth: true
                placeholderText: qsTr("Ext. Fuse")
            }
        }

        Button {
            id: saveHexBtn
            text: qsTr("Save HEX File")
            Layout.fillWidth: true
        }
    }
}
