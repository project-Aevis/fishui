import QtQuick
import QtQuick.Templates as T
import FishUI 1.0 as FishUI

T.TabButton {
    id: control

    property int standardHeight: FishUI.Units.iconSizes.medium + FishUI.Units.smallSpacing
    property color pressedColor: Qt.rgba(FishUI.Theme.textColor.r, FishUI.Theme.textColor.g, FishUI.Theme.textColor.b, 0.5)

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                             standardHeight)
    baselineOffset: contentItem.y + contentItem.baselineOffset

    padding: 0
    spacing: 0

    contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        text: control.text
        color: !control.enabled ? FishUI.Theme.disabledTextColor
                                : control.pressed ? pressedColor
                                : control.checked ? FishUI.Theme.textColor
                                                  : (FishUI.Theme.darkMode ? Qt.rgba(1, 1, 1, 0.65) : Qt.rgba(0, 0, 0, 0.55))
        font.weight: control.checked ? Font.DemiBold : Font.Normal
    }
}
