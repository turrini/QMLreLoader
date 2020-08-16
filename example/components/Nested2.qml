import QtQuick 2.0

Rectangle {
    anchors.fill: parent
    color: "green"

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Nested2.qml datetime -> " + new Date()
        font.pixelSize: 14

        color: "white"
    }
}
