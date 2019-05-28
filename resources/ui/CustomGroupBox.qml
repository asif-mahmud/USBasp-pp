import QtQuick 2.2
import QtQuick.Controls 2.2

GroupBox {
    id: boxItem
    width: parent.width
    height: parent.height
    property bool darkTheme: true
    property int borderRadius: 3

    background: Rectangle {
        y: boxItem.topPadding - boxItem.bottomPadding
        width: parent.width
        height: parent.height - boxItem.topPadding + boxItem.bottomPadding
        color: "transparent"
        border.color: boxItem.darkTheme ? "#ffffff" : "#000000"
        radius: boxItem.borderRadius
    }

    label: Label{
        width: boxItem.width
        text: boxItem.title
        font.capitalization: Font.AllUppercase
        font.underline: true
        horizontalAlignment: Text.AlignHCenter
    }
}
