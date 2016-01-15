import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 400
    height: 600
    title: qsTr("Great Esc")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15

        TextField {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: "p.takashiro.me:5526"
            placeholderText: qsTr("Server")
            font.pointSize: 20
        }

        TextField {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            placeholderText: qsTr("User Name")
            font.pointSize: 20
        }

        TextField {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            echoMode: TextInput.Password
            placeholderText: qsTr("Password")
            font.pointSize: 20
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: qsTr("Login")
        }
    }
}
