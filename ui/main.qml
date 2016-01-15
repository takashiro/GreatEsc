import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 400
    height: 340
    title: qsTr("Great Esc")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15

        TextField {
            id: serverEdit
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: Config.server
            placeholderText: qsTr("Server")
            font.pointSize: 20
        }

        TextField {
            id: userNameEdit
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: Config.account
            placeholderText: qsTr("User Name")
            font.pointSize: 20
        }

        TextField {
            id: passwordEdit
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            echoMode: TextInput.Password
            text: Config.password
            placeholderText: qsTr("Password")
            font.pointSize: 20
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: qsTr("Setup")

            onClicked: {
                Config.server = serverEdit.text;
                Config.account = userNameEdit.text;
                Config.password = passwordEdit.text;
            }
        }
    }
}
