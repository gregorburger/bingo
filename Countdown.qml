// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: mainwindow.width_
    height: mainwindow.height_
    color: "#ea007b"

    Text {
        color: "black"
        anchors.fill: parent
        font.pixelSize: parent.height * 0.7
        text: mainwindow.countDown
        font.family: "Arial Black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
