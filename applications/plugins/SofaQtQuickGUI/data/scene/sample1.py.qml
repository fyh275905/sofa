import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import SofaBasics 1.0

CollapsibleGroupBox {
    id: root

    title: "Scene Parameters"
    enabled: scene.ready

    ColumnLayout {
        anchors.fill: parent

        GroupBox {
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 3

                Item {
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    Layout.preferredHeight: aboutLabel.implicitHeight

                    Label {
                        id: aboutLabel
                        anchors.fill: parent

                        wrapMode: Text.WordWrap
                        text: "<b>About ?</b><br />
                            The goal of this example is to show you how to get / set Sofa data directly from the Scene QML object.<br />"
                    }
                }

                Label {
                    Layout.preferredWidth: implicitWidth
                    text: ""
                }
                RowLayout {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true

                    Label {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 20
                        horizontalAlignment: Text.AlignHCenter
                        text: "X    "
                        color: enabled ? "red" : "darkgrey"
                        font.bold: true
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 20
                        horizontalAlignment: Text.AlignHCenter
                        text: "Y    "
                        color: enabled ? "green" : "darkgrey"
                        font.bold: true
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 20
                        horizontalAlignment: Text.AlignHCenter
                        text: "Z    "
                        color: enabled ? "blue" : "darkgrey"
                        font.bold: true
                    }
                }

                // gravity
                Label {
                    Layout.preferredWidth: implicitWidth
                    text: "Gravity"
                }
                Vector3DSpinBox {
                    id: gravity
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    decimals: 4
                    stepSize:0.01

                    function update() {
                        scene.setData("@.gravity", [[vx, vy, vz]])
                    }

                    Component.onCompleted: {
                        setValueFromArray(scene.getData("@.gravity")[0]);

                        onVxChanged.connect(update);
                        onVyChanged.connect(update);
                        onVzChanged.connect(update);
                    }
                }

                // point location
                Label {
                    Layout.preferredWidth: implicitWidth
                    text: "Position"
                }
                Vector3DSpinBox {
                    id: pointLocation
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    decimals: 4
                    stepSize:0.01
                    enabled: false

                    Timer {
                        id: timer
                        running: false
                        repeat: true
                        interval: 20

                        onTriggered: if(scene.ready) pointLocation.update()
                    }

                    function update() {
                        setValueFromArray(scene.getData("@P0/M0.position")[1]);

                        // this is how to set new positions:
                        //scene.setData("@P0/M0.position", [[3, 2, 1], [6, 5, 4]]);
                    }

                    Component.onCompleted: timer.start()
                }
            }
        }
    }
}
