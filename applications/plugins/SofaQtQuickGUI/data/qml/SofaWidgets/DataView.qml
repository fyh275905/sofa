import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.settings 1.0
import SofaBasics 1.0
import Scene 1.0
import "qrc:/SofaCommon/SofaSettingsScript.js" as SofaSettingsScript

Rectangle {
    id: root
    width: 256
    height: 256
    color: "lightgrey"

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        SofaSettingsScript.Ui.replace(previousUiId, uiId);
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property string sceneSource
        property string sceneDataPath
    }

    function init() {
        uiSettings.sceneSource      = Qt.binding(function() {return sceneSource;});
        uiSettings.sceneDataPath    = Qt.binding(function() {return sceneDataPath;});
    }

    function load() {
        if(0 === uiId)
            return;

        sceneSource     = uiSettings.sceneSource;
        sceneDataPath   = uiSettings.sceneDataPath;
    }

    function setNoSettings() {
        SofaSettingsScript.Ui.remove(uiId);
        uiId = 0;
    }

    Component.onCompleted: {
        if(0 === uiId)
            uiId = SofaSettingsScript.Ui.generate();
        else
            load();

        init();
    }

    property Scene  scene
    property string sceneSource: ""
    property string sceneDataPath: ""

    QtObject {
        id : d

        property QtObject sceneData
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Data {
                anchors.fill: parent

                readOnly: true
                showName: false
                showLinkButton: false
                showTrackButton: false

                scene: root.scene
                sceneData: d.sceneData
            }

            Text {
                anchors.fill: parent
                visible: d.sceneData ? false : true
                text: "No data at this path"
                color: "darkred"
                font.bold: true
                font.pixelSize: 22
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 22
            color: "lightgrey"

            Flickable {
                id: toolBarFlickable
                anchors.fill: parent
                contentWidth: toolBarLayout.implicitWidth
                leftMargin: 32

                RowLayout {
                    id: toolBarLayout
                    height: parent.height
                    spacing: 2

                    TextField {
                        id: dataPathTextField
                        Layout.fillWidth: true
                        Layout.preferredWidth: toolBarFlickable.width / 2.0
                        placeholderText: "@./dataPath from root node"
                        textColor: d.sceneData ? "green" : "black"

                        onTextChanged: d.sceneData = root.scene.data(dataPathTextField.text)
                    }
                }
            }
        }
    }
}
