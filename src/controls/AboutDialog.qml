/*
 * Copyright (C) 2021 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import FishUI 1.0 as FishUI

FishUI.Window {
    id: control

    width: 300
    height: contentHeight

    maximumWidth: control.width
    minimumWidth: control.width

    maximumHeight: contentHeight
    minimumHeight: contentHeight

    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.FramelessWindowHint

    minimizeButtonVisible: false
    visible: false

    property var iconSource
    property string name
    property string description
    property string attribution: qsTr("Project Aevis — Built upon the foundation of Cutefish OS")
    property string link: "https://github.com/project-aevis"
    property var contentHeight: _mainLayout.implicitHeight + control.header.height * 2

    background.opacity: FishUI.Theme.blurEnabled ? (FishUI.Theme.darkMode ? 0.75 : 0.78) : 1

    ColumnLayout {
        id: _mainLayout
        anchors.fill: parent
        anchors.bottomMargin: control.header.height

        Image {
            width: 64
            height: 64
            source: control.iconSource
            sourceSize: Qt.size(64, 64)
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: FishUI.Units.largeSpacing
        }

        Label {
            text: control.name
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
        }

        Label {
            text: control.description
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: control.attribution
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 9
            color: FishUI.Theme.disabledTextColor
            visible: control.attribution.length > 0
        }

        Label {
            text: "<a href=\"%1\">%1</a>".arg(control.link)
            Layout.alignment: Qt.AlignHCenter
            linkColor: FishUI.Theme.highlightColor

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: Qt.openUrlExternally(control.link)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
