import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

CustomGroupBox {
    id: actionBox
    title: qsTr("step 3")

    ColumnLayout {
        width: actionBox.availableWidth

        RowLayout {
            width: parent.availableWidth

            ColumnLayout {

                Button {
                    text: qsTr("Read Flash")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Read LFuse")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Read HFuse")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Read EFuse")
                    Layout.fillWidth: true
                }
            } // left side buttons

            ColumnLayout {

                Button {
                    text: qsTr("Write Flash")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Write LFuse")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Write HFuse")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Write EFuse")
                    Layout.fillWidth: true
                }
            } // right side buttons
        } // upper row with read and write buttons

        Button {
            text: qsTr("Chip Erase")
            Layout.fillWidth: true
        }
    }
}
