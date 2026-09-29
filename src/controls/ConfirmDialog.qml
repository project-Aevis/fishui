import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import FishUI 1.0 as FishUI

FishUI.Window {
    id: control

    property string text
    property string cancelText: qsTr("Cancel")
    property string confirmText: qsTr("Confirm")
    property bool destructive: false
    property bool confirmEnabled: true

    signal accepted()
    signal rejected()

    flags: Qt.Dialog | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    modality: Qt.WindowModal
    visible: false
    minimizeButtonVisible: false
    windowButtonsVisible: false
    contentTopMargin: 0
    windowRadius: 0
    header.height: 0
    background.color: FishUI.Theme.darkMode ? Qt.rgba(0.04, 0.04, 0.08, 0.22) : Qt.rgba(0.92, 0.92, 0.96, 0.18)
    background.opacity: 1.0

    readonly property int dialogWidth: Math.max(380, actions.implicitWidth + 40)
    readonly property int dialogHeight: layout.implicitHeight + 40
    readonly property int textWidth: dialogWidth - 40

    width: Screen.width
    height: Screen.height
    minimumWidth: Screen.width
    maximumWidth: Screen.width
    minimumHeight: Screen.height
    maximumHeight: Screen.height

    x: 0
    y: 0

    property bool _accepted: false

    function accept() {
        if (!confirmEnabled)
            return
        _accepted = true
        accepted()
        close()
    }

    onClosing: {
        if (!_accepted)
            rejected()
    }
    onVisibleChanged: {
        if (visible) {
            _accepted = false
            cancelButton.forceActiveFocus()
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: control.close()
    }

    // Dismiss overlay when clicking on background outside the dialog card
    MouseArea {
        anchors.fill: parent
        onClicked: control.close()
    }

    // Centered Translucent Glass Dialog Card
    Rectangle {
        id: dialogCard
        anchors.centerIn: parent
        width: control.dialogWidth
        height: control.dialogHeight
        radius: FishUI.Theme.bigRadius
        color: FishUI.Theme.darkMode
               ? Qt.rgba(0.12, 0.12, 0.16, 0.75)
               : Qt.rgba(0.97, 0.97, 0.99, 0.80)

        border.width: 1 / Screen.devicePixelRatio
        border.color: FishUI.Theme.glassBorderColor

        // Specular glass highlight at the top border
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: parent.radius
            anchors.rightMargin: parent.radius
            height: 1
            color: FishUI.Theme.glassSpecularColor
        }

        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            radius: 28
            samples: 29
            horizontalOffset: 0
            verticalOffset: 8
            color: Qt.rgba(0, 0, 0, 0.30)
        }

        // Prevent clicks inside the card from reaching the backdrop MouseArea
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }

        ColumnLayout {
            id: layout
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            Column {
                Layout.fillWidth: true
                spacing: 6

                QQC2.Label {
                    width: control.textWidth
                    text: control.title
                    textFormat: Text.PlainText
                    font.family: FishUI.Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    color: FishUI.Theme.textColor
                    wrapMode: Text.Wrap
                }

                QQC2.Label {
                    width: control.textWidth
                    text: control.text
                    textFormat: Text.PlainText
                    font.family: FishUI.Theme.fontFamily
                    font.pixelSize: 13
                    color: FishUI.Theme.disabledTextColor
                    wrapMode: Text.Wrap
                    lineHeight: 18
                    lineHeightMode: Text.FixedHeight
                    visible: text.length > 0
                }
            }

            RowLayout {
                id: actions
                Layout.alignment: Qt.AlignRight
                spacing: 8

                FishUI.ConfirmDialogButton {
                    id: cancelButton
                    text: control.cancelText
                    focus: true
                    onClicked: control.close()
                }

                FishUI.ConfirmDialogButton {
                    text: control.confirmText
                    primary: true
                    destructive: control.destructive
                    enabled: control.confirmEnabled
                    onClicked: control.accept()
                }
            }
        }
    }
}
