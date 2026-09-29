import QtQuick
import FishUI 1.0 as FishUI

Item {
    id: indicator
    implicitWidth: 42
    implicitHeight: 28

    property Item control
    property alias handle: handle
    property color bgColor: control.checked ? FishUI.Theme.highlightColor
                                            : FishUI.Theme.darkMode ? Qt.rgba(1, 1, 1, 0.20)
                                                                    : Qt.rgba(0, 0, 0, 0.16)

    Rectangle {
        width: parent.width - 2
        height: 22
        x: 1
        radius: height / 2
        y: parent.height / 2 - height / 2
        color: bgColor
        opacity: control.enabled ? 1 : 0.55
        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.InOutCubic
            }
        }
    }

    Rectangle {
        id: handle
        x: 2 + control.visualPosition * (parent.width - width - 4)
        y: (parent.height - height) / 2
        width: 18
        height: 18
        radius: width / 2
        color: "white"
        opacity: control.enabled ? 1 : 0.65
        scale: control.pressed ? 0.8 : 1

        Behavior on scale {
            NumberAnimation {
                duration: 100
            }
        }

        Behavior on x {
            enabled: !control.pressed

            NumberAnimation {
                duration: 250
                easing.type: Easing.OutSine
            }
        }
    }
}
