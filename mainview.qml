import QtQuick 1.1

Rectangle {
    id: root_rectangle
    //color: "#8e8ed7"
    width: mainwindow.width_
    height: mainwindow.height_
    
    Text {
        width: 400
        height: parent.height
        color: "green"
        text: mainwindow.last_number
        font.family: "Helveticy"
        font.pixelSize: parent.height - 30
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    
    Rectangle {
        //border.color: "black"
        //border.width: 2
        width: 40
        height: parent.height
        anchors.left: parent.left
        ListView {
            anchors.fill: parent
            model: mainwindow.old_numbers
            delegate: Text {
                font.pixelSize: 30
                color: "red"
                width: parent.width
                height: 35
                text: modelData
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}

