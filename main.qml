import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import QtQuick.Dialogs 1.2

import base.QmldirTree 1.0

Window {
    id: root

    property QmldirTree qmldirTree: qmldir_tree

    visible: true
    width: 1280
    height: 720
    title: "QmlImportReviewer"

    RowLayout {
        id: _controlsRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        height: 30

        Button {
            id: _directory

            Layout.fillHeight: true
            Layout.fillWidth: true

            enabled: !qmldirTree.searching_in_progress

            text: "Root qml dir: " + "<b>" + qmldir_tree.qml_folder_string + "<\b>"

            onClicked: {
                _folderDialog.open();
            }
        }

        Button {
            id: _searchingQmldir

            Layout.fillHeight: true

            enabled: !qmldirTree.searching_in_progress

            text: "Search qmldirs"

            onClicked: {
                qmldirTree.start_searching(qmldir_tree.qml_folder_string);
            }
        }

        Button {
            id: _deleteImports

            Layout.fillHeight: true

            enabled: !qmldirTree.searching_in_progress

            text: "Delete imports"

            onClicked: {
                qmldirTree.delete_unusable_imports();
            }
        }

        Button {
            id: _deleteQmlFiles

            Layout.fillHeight: true

            enabled: !qmldirTree.searching_in_progress

            text: "Delete Qml files"

            onClicked: {
                qmldirTree.delete_unusable_qml_files();
            }
        }
    }

//    ListView {
//        id : _listView

//        anchors.top: _controlsRow.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
//        anchors.bottom: parent.bottom

//        model: 20
//        clip: true
//        spacing: 5

//        delegate: Rectangle {
//            id: _delegate
//            height: 20
//            width: parent.width
//            color: "red"
//            Text {
//                id: _fileName
////                text: model.fileName
//            }
//        }
//    }

    FileDialog {
        id: _folderDialog

        folder: qmldirTree.qml_folder_string
        selectFolder: true

        onAccepted: {
            qmldirTree.set_qml_folder_string(folder, false);
        }

        Component.onCompleted: {
            qmldirTree.set_qml_folder_string("////////home/hetman/WORK/PROJECTS/delorean/delorean/hmi");
//            qmldirTree.set_qml_folder_string("////////home/thetman/Project/donington/ha-hmi/src/hmi/qml");
//            qmldirTree.set_qml_folder_string(shortcuts.home, false); // TODO set on release
        }
    }
}
