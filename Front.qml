import QtQuick 1.0

Rectangle {
    id: root_rectangle
    color: "#ea007b"

    Text {
        width: 400
        height: parent.height
        color: "black"
        text: mainwindow.last_number
        font.family: "Ariel Black"
        font.pixelSize: root_rectangle.height * 0.8
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        //border.color: "black"
        //border.width: 2
        width: root_rectangle.height/10
        height: parent.height
        color: "#ea007b"
        x: 2+root_rectangle.width/40
        y: 2
        ListView {
            anchors.fill: parent
            model: mainwindow.old_numbers
            delegate: Rectangle {
                width: parent.width
                height: root_rectangle.height/10
                border.color: "black"
                border.width: 4
                radius: 10
                Text {
                    font.pixelSize: root_rectangle.height/15
                    color: "black"
                    anchors.fill: parent
                    text: modelData
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.family: "Ariel Black"
                }
            }
        }
    }
}
