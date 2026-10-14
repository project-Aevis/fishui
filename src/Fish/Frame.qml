import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import FishUI 1.0 as FishUI

T.Frame {
    id: control

    implicitWidth: contentWidth + leftPadding + rightPadding
    implicitHeight: contentHeight + topPadding + bottomPadding

    contentWidth: contentItem.implicitWidth || (contentChildren.length === 1 ? contentChildren[0].implicitWidth : 0)
    contentHeight: contentItem.implicitHeight || (contentChildren.length === 1 ? contentChildren[0].implicitHeight : 0)

    padding: 6

    background: Rectangle {
        color: "transparent"
        radius: FishUI.Theme.smallRadius
        border.width: 1
        border.color: FishUI.Theme.glassBorderColor
    }
}
