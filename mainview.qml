import QtQuick 1.0

Flipable {
    id: main
    width: bingowindow.width_
    height: bingowindow.height_
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
            source: mainwindow.back_image
        }
    }

    transform: Rotation {
        id: rotation
        origin.x: main.width/2
        origin.y: main.height/2
        axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
        angle: 0    // the default angle
    }



    states: [State {
        name: "backbingo"
        PropertyChanges { target: rotation; angle: 180 }
        when: mainwindow.bingo
    }, State {
        name: "backwedgie"
        PropertyChanges { target: rotation; angle: 180 }
        when: mainwindow.wedgie
    }]

    transitions: Transition {
        NumberAnimation { target: rotation; property: "angle"; duration: 250 }
    }

    focus: true
}
