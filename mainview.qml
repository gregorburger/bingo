import QtQuick 1.0

Flipable {
    id: main
    width: mainwindow.width_
    height: mainwindow.height_
    property bool flipped: false

    front: Front {
        anchors.fill: parent
    }

    back: Rectangle {
        color: "#ea007b"
        anchors.fill: parent
        Image {
            id: back_image
            anchors.fill: parent
            smooth: false
            fillMode: Image.PreserveAspectCrop

        }
    }

    transform: Rotation {
        id: rotation
        origin.x: main.width/2
        origin.y: main.height/2
        axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
        angle: 0    // the default angle
    }



    states: State {
        name: "back"
        PropertyChanges { target: rotation; angle: 180 }
        when: main.flipped
    }

    transitions: Transition {
        NumberAnimation { target: rotation; property: "angle"; duration: 250 }
    }

    MouseArea {
        anchors.fill: parent
        //onClicked: main.flipped = !main.flipped
    }

    focus: true
    Keys.onPressed: {
        if (event.key == Qt.Key_B) {
            back_image.source = "qrc:/imgs/imgs/bingo.png"
            main.flipped = !main.flipped
        }
        if (event.key == Qt.Key_W) {
            back_image.source = "qrc:/imgs/imgs/wedgie.jpg"
            main.flipped = !main.flipped
        }
    }
}
