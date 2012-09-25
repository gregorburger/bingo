// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: bingowindow.width_
    height: bingowindow.height_
    color: "#ea007b"

    Text {
        text : "You have..."
        width: parent.width
        height: parent.height * 0.15
        font.pixelSize: height
        font.family: "Ariel Black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    
    Text {
        color: "black"
        anchors.fill: parent
        font.pixelSize: parent.height * 0.45
        text: mainwindow.countDown
        font.family: "Ariel Black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    
    Text {
        text : "left to Piss!"
        width: parent.width
        height: parent.height * 0.15
        font.pixelSize: height
        font.family: "Ariel Black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.bottom: parent.bottom
    }
}
