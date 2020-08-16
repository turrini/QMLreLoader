import QtQuick 2.0

Rectangle {
    anchors.fill: parent
    color: "red"

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Nested1.qml datetime -> " + new Date()
        font.pixelSize: 14

        color: "white"
    }

    Loader {
        id: anotherLoader

        anchors.centerIn: parent

        width: 450
        height: 100

        source: "qrc:/components/Nested2.qml"

        Component.onCompleted: {
            reloader.registerLoader(anotherLoader);
        }
    }
}
