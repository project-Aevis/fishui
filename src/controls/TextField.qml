/*
 * Copyright (C) 2021 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
 *
 * Author:     revenmartin <revenmartin@gmail.com>
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

import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import FishUI 1.0 as FishUI

Controls.TextField {
    id: control

    implicitWidth: Math.max(200, placeholderText ? (contentWidth + leftPadding + rightPadding) : 0)
                            || (contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding,
                             background ? background.implicitHeight : 0)

    leftPadding: FishUI.Units.smallSpacing + 2
    rightPadding: FishUI.Units.smallSpacing + 2

    renderType: FishUI.Theme.renderType
    color: control.enabled ? FishUI.Theme.textColor : FishUI.Theme.disabledTextColor
    selectionColor: FishUI.Theme.highlightColor
    selectedTextColor: FishUI.Theme.highlightedTextColor
    selectByMouse: true
    verticalAlignment: TextInput.AlignVCenter

    placeholderTextColor: FishUI.Theme.disabledTextColor

    background: Rectangle {
        implicitWidth: (FishUI.Units.iconSizes.medium * 3) + FishUI.Units.smallSpacing
        implicitHeight: FishUI.Units.iconSizes.medium + FishUI.Units.smallSpacing
        color: FishUI.Theme.alternateBackgroundColor
        radius: FishUI.Theme.smallRadius

        border.width: 1
        border.color: control.activeFocus ? FishUI.Theme.highlightColor : FishUI.Theme.glassBorderColor

        Behavior on border.color {
            ColorAnimation {
                duration: 150
            }
        }
    }
}