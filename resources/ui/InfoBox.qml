import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

CustomGroupBox {
    id: infoBox
    title: qsTr("Current Configuration")
    property string currentHexFile: qsTr("none")
    property bool showFullHexPath: showFullHexPathSw.checked

    function setShowFullHexPathSw(flag) {
        showFullHexPathSw.checked = flag
    }

    ColumnLayout {
        width: infoBox.availableWidth

        RowLayout {
            Label {
                Layout.fillWidth: true
                text: qsTr("Current HEX file: ") + infoBox.currentHexFile
                wrapMode: Text.WrapAnywhere
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Current Command: ") + infoBox.currentHexFile
                wrapMode: Text.WrapAnywhere
            }
        }

        RowLayout {
            Switch {
                id: showFullHexPathSw
                text: qsTr("Show Full HEX Path")
            }

            Label {
                text: qsTr("Extra Arguments: ")
            }

            TextField {
                Layout.fillWidth: true
            }
        }
    }
}
