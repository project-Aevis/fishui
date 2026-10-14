/*
 * Copyright (C) 2021 CutefishOS Team.
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

pragma Singleton

import QtQuick 2.4
import FishUI.Core 1.0 as FishUICore

QtObject {
    id: theme

    property bool darkMode: FishUICore.ThemeManager.darkMode
    property bool blurEnabled: FishUICore.ThemeManager.blurEnabled

    property color blueColor: FishUICore.ThemeManager.blueColor
    property color redColor: FishUICore.ThemeManager.redColor
    property color greenColor: FishUICore.ThemeManager.greenColor
    property color purpleColor: FishUICore.ThemeManager.purpleColor
    property color pinkColor: FishUICore.ThemeManager.pinkColor
    property color orangeColor: FishUICore.ThemeManager.orangeColor
    property color greyColor: FishUICore.ThemeManager.greyColor

    property color backgroundColor: darkMode ? "#18181A" : "#F1F3F5"
    property color secondBackgroundColor: darkMode ? "#242426" : "#FFFFFF"
    property color alternateBackgroundColor: darkMode ? "#2E2E32" : "#EAECEF"

    property color textColor: darkMode ? "#FFFFFF" : "#1A1A1A"
    property color disabledTextColor: darkMode ? "#8A8A8E" : "#8E8E93"

    property color highlightColor: FishUICore.ThemeManager.accentColor
    property color highlightedTextColor: darkMode ? "#FFFFFF" : "#FFFFFF"

    property color activeTextColor: "#007AFF"
    property color activeBackgroundColor: "#007AFF"

    property color linkColor: "#007AFF"
    property color linkBackgroundColor: "#007AFF"
    property color visitedLinkColor: "#007AFF"
    property color visitedLinkBackgroundColor: "#007AFF"

    property real fontSize: FishUICore.ThemeManager.fontSize
    property string fontFamily: FishUICore.ThemeManager.fontFamily

    property real smallRadius: 8.0
    property real mediumRadius: 12.0
    property real bigRadius: 16.0
    property real hugeRadius: 20.0
    property real windowRadius: 24.0

    property color glassBorderColor: darkMode ? Qt.rgba(1, 1, 1, 0.18) : Qt.rgba(1, 1, 1, 0.85)
    property color glassSpecularColor: darkMode ? Qt.rgba(1, 1, 1, 0.12) : Qt.rgba(1, 1, 1, 0.60)
    property color glassShadowColor: Qt.rgba(0, 0, 0, darkMode ? 0.45 : 0.20)

    property var renderType: Text.QtRendering

    property font defaultFont: fontMetrics.font
    property font smallFont: {
        let font = fontMetrics.font
        if (font.pixelSize > 0) {
            font.pixelSize = Math.max(1, font.pixelSize - 1)
        } else {
            font.pointSize = Math.max(1, font.pointSize - 1)
        }
        return font
    }

    property list<QtObject> children: [
        TextMetrics {
            id: fontMetrics
            font.family: theme.fontFamily
            font.pointSize: theme.fontSize
        }
    ]

    // Whether the icon theme draws this icon as a single-colour glyph, so it
    // may be recoloured to match its surroundings.
    function isMonochromeIcon(name) {
        return FishUICore.ThemeManager.isMonochromeIcon(name)
    }
}
