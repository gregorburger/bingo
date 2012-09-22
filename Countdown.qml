// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: bingowindow.width_
    height: bingowindow.height_
    color: "#ea007b"

    Text {
        color: "black"
        anchors.fill: parent
        font.pixelSize: parent.height * 0.6
        text: mainwindow.countDown
        font.family: "Ariel Black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
