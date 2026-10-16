/*
 * Copyright (C) 2021 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
 *
 * Author:     Reion Wong <reion@cutefishos.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick
import QtQuick.Templates as T
import Qt5Compat.GraphicalEffects
import FishUI 1.0 as FishUI
import QtQuick.Controls.impl

T.Button {
    id: control
    implicitWidth: Math.max(background.implicitWidth, contentItem.implicitWidth + FishUI.Units.largeSpacing)
    implicitHeight: background.implicitHeight
    hoverEnabled: true

    icon.width: FishUI.Units.iconSizes.small
    icon.height: FishUI.Units.iconSizes.small

    icon.color: control.enabled ? (control.highlighted ? "#FFFFFF" : (control.flat ? FishUI.Theme.highlightColor : FishUI.Theme.textColor)) : FishUI.Theme.disabledTextColor
    spacing: FishUI.Units.smallSpacing

    property color hoveredColor: FishUI.Theme.darkMode ? Qt.lighter(FishUI.Theme.alternateBackgroundColor, 1.15)
                                                       : Qt.darker(FishUI.Theme.alternateBackgroundColor, 1.05)

    property color pressedColor: FishUI.Theme.darkMode ? Qt.darker(FishUI.Theme.alternateBackgroundColor, 1.15)
                                                       : Qt.darker(FishUI.Theme.alternateBackgroundColor, 1.12)

    property color borderColor: control.activeFocus ? FishUI.Theme.highlightColor
                                                    : (FishUI.Theme.darkMode ? Qt.rgba(1, 1, 1, 0.08) : Qt.rgba(0, 0, 0, 0.06))

    property color flatHoveredColor: Qt.rgba(FishUI.Theme.highlightColor.r,
                                             FishUI.Theme.highlightColor.g,
                                             FishUI.Theme.highlightColor.b, 0.12)
    property color flatPressedColor: Qt.rgba(FishUI.Theme.highlightColor.r,
                                             FishUI.Theme.highlightColor.g,
                                             FishUI.Theme.highlightColor.b, 0.20)

    contentItem: IconLabel {
        text: control.text
        font: control.font
        icon: control.icon
        color: !control.enabled ? FishUI.Theme.disabledTextColor
                                : control.highlighted ? "#FFFFFF"
                                : control.flat ? FishUI.Theme.highlightColor : FishUI.Theme.textColor
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: Qt.AlignCenter
    }

    background: Item {
        implicitWidth: (FishUI.Units.iconSizes.medium * 3) + FishUI.Units.largeSpacing
        implicitHeight: FishUI.Units.iconSizes.medium + FishUI.Units.smallSpacing

        Rectangle {
            id: _flatBackground
            anchors.fill: parent
            radius: FishUI.Theme.smallRadius
            border.width: 1
            border.color: control.enabled && control.activeFocus ? FishUI.Theme.highlightColor : "transparent"
            visible: control.flat

            color: {
                if (!control.enabled)
                    return "transparent"

                if (control.pressed)
                    return control.flatPressedColor

                if (control.hovered)
                    return control.flatHoveredColor

                return "transparent"
            }
        }

        Rectangle {
            id: _background
            anchors.fill: parent
            radius: FishUI.Theme.smallRadius
            border.width: 1
            visible: !control.flat
            border.color: control.enabled ? control.borderColor : "transparent"

            color: {
                if (!control.enabled)
                    return FishUI.Theme.alternateBackgroundColor

                if (control.highlighted) {
                    if (control.pressed)
                        return Qt.darker(FishUI.Theme.highlightColor, 1.15)
                    if (control.hovered)
                        return Qt.lighter(FishUI.Theme.highlightColor, 1.10)
                    return FishUI.Theme.highlightColor
                }

                if (control.pressed)
                    return control.pressedColor

                if (control.hovered)
                    return control.hoveredColor

                return FishUI.Theme.alternateBackgroundColor
            }
        }
    }
}
