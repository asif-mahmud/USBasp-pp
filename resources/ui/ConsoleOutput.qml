import QtQuick 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2

CustomGroupBox {
    id: consoleOutput
    title: qsTr("console output")
    property bool preserveOutput: preserveOutputSw.checked

    function setPreserveOutputSw(flag) {
        preserveOutputSw.checked = flag
    }

    ColumnLayout {

        Switch {
            id: preserveOutputSw
            text: qsTr("Preserve output")
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextEdit {
                id: output
                readOnly: true
            }
        }
    }
}
