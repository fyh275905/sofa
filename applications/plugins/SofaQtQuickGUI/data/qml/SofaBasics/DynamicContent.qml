import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.2
import Qt.labs.folderlistmodel 2.1
import Qt.labs.settings 1.0

Item {
    id: root
    clip: true

    readonly property bool isDynamicContent: true

    property int uiId: 0
    property int previousUiId: uiId
    onUiIdChanged: {
        globalUiSettings.replaceUiId(previousUiId, uiId);
    }

    Settings {
        id: uiSettings
        category: 0 !== root.uiId ? "ui_" + root.uiId : "dummy"

        property string sourceDir
        property string defaultContentName
        property string currentContentName
        property int    contentUiId
    }

    function init() {
        uiSettings.contentUiId          = Qt.binding(function() {return root.contentUiId;});
        uiSettings.sourceDir            = Qt.binding(function() {return root.sourceDir;});
        uiSettings.defaultContentName   = Qt.binding(function() {return root.defaultContentName;});
        uiSettings.currentContentName   = Qt.binding(function() {return root.currentContentName;});
    }

    function load() {
        if(0 === uiId)
            return;

        root.contentUiId        = uiSettings.contentUiId;
        root.sourceDir          = uiSettings.sourceDir;
        root.defaultContentName = uiSettings.defaultContentName;
        root.currentContentName = uiSettings.currentContentName;
    }

    function setNoSettings() {
        globalUiSettings.removeUiId(uiId);
        uiId = 0;
    }

    property string defaultContentName
    property string currentContentName
    property string sourceDir: "qrc:/SofaWidgets"
    property int    contentUiId: 0

    onSourceDirChanged: update()
    onCurrentContentNameChanged: update()
    Component.onCompleted: {
        if(0 === root.uiId) {
            if(parent && undefined !== parent.contentUiId && 0 !== parent.contentUiId) {
                root.uiId = parent.contentUiId;
                load();
            }
            else
                root.uiId = globalUiSettings.generateUiId();
        }
        else
            load();

        init();

        update();
    }

    function update() {
        folderListModel.folder = ""; // we must reset the folder property
        folderListModel.folder = sourceDir;
    }

    FolderListModel {
        id: folderListModel
        nameFilters: ["*.qml"]
        showDirs: false
        showFiles: true
        sortField: FolderListModel.Name

        function update() {
            listModel.clear();

            var contentSet = false;

            var previousIndex = comboBox.currentIndex;
            for(var i = 0; i < count; ++i)
            {
                var fileBaseName = get(i, "fileBaseName");
                var filePath = get(i, "filePath").toString();
                if(-1 !== folder.toString().indexOf("qrc:"))
                    filePath = "qrc" + filePath;

                listModel.insert(i, {"fileBaseName": fileBaseName, "filePath": filePath});

                if(0 === root.currentContentName.localeCompare(fileBaseName)) {
                    comboBox.currentIndex = i;
                    contentSet = true;
                }
            }

            if(!contentSet) {
                for(var i = 0; i < count; ++i)
                {
                    var fileBaseName = get(i, "fileBaseName");

                    if(0 === defaultContentName.localeCompare(fileBaseName)) {
                        comboBox.currentIndex = i;
                        break;
                    }
                }
            }

            if(count > 0 && previousIndex === comboBox.currentIndex)
                loaderLocation.refresh();
        }

        onCountChanged: {
            update();
        }
    }

    readonly property alias contentItem: loaderLocation.contentItem
    Item {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                id: loaderLocation
                Layout.fillWidth: true
                Layout.fillHeight: true

                property Item contentItem

                onContentItemChanged: {
                    refreshStandbyItem();
                }

                property string errorMessage
                function refresh() {
                    if(-1 === comboBox.currentIndex || comboBox.currentIndex >= listModel.count)
                        return;

                    var currentData = listModel.get(comboBox.currentIndex);
                    if(currentData) {
                        var source = listModel.get(comboBox.currentIndex).filePath;

                        if(root.currentContentName === comboBox.currentText && null !== loaderLocation.contentItem)
                            return;

                        root.currentContentName = comboBox.currentText;

                        if(loaderLocation.contentItem) {
                            if(undefined !== loaderLocation.contentItem.setNoSettings)
                                loaderLocation.contentItem.setNoSettings();

                            loaderLocation.contentItem.destroy();
                            loaderLocation.contentItem = null;
                        }

                        var contentComponent = Qt.createComponent(source);
                        if(contentComponent.status === Component.Error) {
                            loaderLocation.errorMessage = contentComponent.errorString();
                            refreshStandbyItem();
                        } else {
                            if(0 === root.contentUiId)
                                root.contentUiId = globalUiSettings.generateUiId();

                            var content = contentComponent.createObject(loaderLocation, {"uiId": root.contentUiId, "anchors.fill": loaderLocation});

                            if(undefined !== content.uiId)
                                root.contentUiId = Qt.binding(function() {return content.uiId;});
                            else
                            {
                                globalUiSettings.removeUiId(root.contentUiId);
                                root.contentUiId = 0;
                            }

                            loaderLocation.contentItem = content;
                        }
                    }
                }

                function refreshStandbyItem() {
                    if(contentItem) {
                        timer.stop();
                        standbyItem.visible = false;
                    } else {
                        timer.start();
                    }
                }
            }

            Rectangle {
                id: toolBar
                Layout.fillWidth: true
                Layout.preferredHeight: visible ? 22 : 0
                color: "lightgrey"
                visible: toolBarMode.displayToolbar

                MouseArea {
                    anchors.fill: parent

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 32

                        ComboBox {
                            id: comboBox
                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 20
                            textRole: "fileBaseName"
                            style: ComboBoxStyle {}

                            model: ListModel {
                                id: listModel
                            }

                            onCurrentIndexChanged: {
                                loaderLocation.refresh();
                            }
                        }
                    }
                }
            }
        }

        Image {
            id: toolBarMode
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.bottomMargin: 3
            anchors.leftMargin: 16
            source: displayToolbar ? "qrc:/icon/minus.png" : "qrc:/icon/plus.png"
            width: 12
            height: width

            property bool displayToolbar: false
            MouseArea {
                anchors.fill: parent
                onClicked: toolBarMode.displayToolbar = !toolBarMode.displayToolbar
            }
        }

        Rectangle {
            id: standbyItem
            anchors.fill: parent
            color: "#555555"
            visible: false

            Label {
                anchors.fill: parent
                color: "red"
                visible: 0 !== loaderLocation.errorMessage.length
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: "An error occurred, the content could not be loaded ! Reason: " + loaderLocation.errorMessage
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }

        Timer {
            id: timer
            interval: 200
            running: false
            repeat: false
            onTriggered: standbyItem.visible = true
        }
    }
}
