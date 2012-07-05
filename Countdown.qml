// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: mainwindow.width_
    height: mainwindow.height_

    Text {
        color: "green"
        anchors.fill: parent
        font.pixelSize: parent.height * 0.7
        text: mainwindow.countDown
        font.family: "Helveticy"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
