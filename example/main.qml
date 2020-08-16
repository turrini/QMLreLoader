import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    width: 640
    height: 360
    visible: true

    title: "QML reLoader Example"

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "main.qml datetime -> " + new Date()
        font.pixelSize: 14
    }

    Loader {
        id: mainLoader

        anchors.centerIn: parent

        width: 500
        height: 200

        source: "qrc:/components/Nested1.qml"

        Component.onCompleted: {
            reloader.registerLoader(mainLoader);
        }
    } // loader
}
