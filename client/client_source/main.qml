import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.VirtualKeyboard 2.1
import a.Core 1.0
import QtQuick.Dialogs 1.1

ApplicationWindow {
    id: window
    visible: true
    width: 540
    height: 630
    title: qsTr("Ftp Client")
    property alias core:core
    Core {
        id: core
    }

    SwipeView {
        id: swipeView
        transformOrigin: Item.Center
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Item {
            id: item1
            width: 600
            height: 320
            property alias listView: listView
            property alias cd: cd
            property alias scrollView: scrollView
            property alias button: button
            property alias item1: item1
            property alias buttonGroup: buttonGroup
            x: 0
            y: 0

            Button {
                id: button
                x: parent.width * 2 / 5 - width / 2
                y: parent.height - 80
                text: qsTr("UPLOAD")
                transformOrigin: Item.Center
                onClicked: {
                    var ret = core.upload()
                    if (ret === Core.NOT_LOGIN)
                    {
                        message.text = "Not login."
                    }
                    else if (ret === Core.FILE_NOT_FOUND)
                    {
                        message.text = "File not found."
                    }
                    else if (ret === Core.ERROR_DATA_CONNECTION)
                    {
                        message.text = "Data connection fail."
                    }
                    else if (ret === Core.ERROR_WRITING)
                    {
                        message.text = "Error Writing."
                    }
                    else if (ret === Core.IS_DIR)
                    {
                        message.text = "Cannot upload directory"
                    }

                    if (ret !== Core.OK)
                    {
                        message.title = "Upload Fail"
                        message.visible = true;
                    }
                }
            }

            Button {
                id: button1
                x: parent.width * 3 / 5 - width / 2
                y: parent.height - 80
                text: qsTr("DOWNLOAD")
                onClicked: {
                    if (buttonGroup.checkedButton)
                    {
                        core.get(buttonGroup.checkedButton.text);
                        console.log("download")
                    }
                }
            }

            Button {
                id: cancel
                x: parent.width * 4 / 5 - width / 2
                y: parent.height - 80
                text: qsTr("STOP")
                transformOrigin: Item.Center
                onClicked: {
                    if (cancel.text === qsTr("STOP"))
                    {
                        if (core.abort() === Core.OK)
                        {
                            cancel.text = qsTr("CONTINUE")
                        }
                    }
                    else
                    {
                        cancel.text = qsTr("STOP")
                        core._continue(buttonGroup.checkedButton.text)
                    }
                }
            }

            GroupBox {
                id: groupBox
                width: item1.width
                height: item1.height - 120
                title: qsTr("Index")

                ScrollView {
                    id: scrollView
                    x: 0
                    y: 0
                    width: groupBox.width - 40
                    height: groupBox.height - 40

                    ListView {
                        id: listView
                        x: 0
                        y: 0
                        width: scrollView.width
                        height: scrollView.height
                        clip: true
                        highlightRangeMode: ListView.ApplyRange

                        ButtonGroup {
                            id: buttonGroup
                        }

                        delegate: Item {
                            x: 5
                            width: listView.width
                            height: 40

                            RadioDelegate {
                                x: 0
                                width: listView.width * 2 / 3
                                height: 40
                                text: name
                                ButtonGroup.group: buttonGroup
                            }

                            ItemDelegate {
                                x: listView.width * 2 / 3
                                width: listView.width / 3
                                height: 40
                                text: type
                            }
                        }
                        model: ListModel{
                            ListElement {
                                name: "root"
                                type: "directory"
                            }
                        }
                    }
                }
            }

            Button {
                id: cd
                x: parent.width / 5 - width / 2
                y: parent.height - 80
                text: qsTr("CD")
                onClicked: {
                    if (buttonGroup.checkedButton)
                    {
                        var ret = core.cd(buttonGroup.checkedButton.text)
                        console.log(ret);
                        if (ret === Core.OK)
                        {
                            listView.model.clear()
                            for (var i = 0; i < core.listnumber; i++)
                            {
                                listView.model.append({"name": core._Name(i), "type": core._Type(i)})
                            }
                        }
                        else if (ret === Core.ROOT_DIR)
                        {
                            message.text = "This is the root dir."
                            message.title = "Permission denied"
                            message.visible = true;
                        }
                    }
                }
            }
        }

        Configuration {
            textInput.onTextChanged: {
                core.set_defaultdir(textInput.text)
            }
            switch1.onToggled: {
                core.toggle()
                console.log("toggle")
            }
            logout.onClicked: {
                core.logout()
            }
            longin.onClicked: {
                core.set_ClientDir(userName.text)
                core.set_ServerDir(passwd.text)
                var ret = core.login()
                if (ret === Core.NOT_CONNECTED)
                {
                    message.text = "Not connected, please connect to the server first."
                }
                else if (ret === Core.LOGGED)
                {
                    message.text = "Already logged in, logout first if you want to switch user."
                }
                else if (ret === Core.LOGIN_FAIL)
                {
                    message.text = "Login fail. Maybe your username and password do not pair."
                }
                if (ret !== Core.OK)
                {
                    message.title = "Login fail"
                    message.visible = true;
                }
            }
            disconnect.onClicked: {
                core.disconnect();
            }
            connect.onClicked: {
                core.set_ip_addr(ipInput.text)
                core.set_port(portInput.text)
                var ret = core.connect()
                if (ret !== Core.OK)
                {
                    message.title = "Connect Fail"
                }
                if (ret === Core.ERROR_CREATE_SOCKET)
                {
                    message.text = "Create socket fail."
                }
                else if (ret === Core.WRONG_IP)
                {
                    message.text = "Wrong IP."
                }
                else if (ret === Core.ERROR_CONNECT)
                {
                    message.text = "Fail to connect, check if the server works well."
                }
                else if (ret === Core.CONNECTED)
                {
                    message.text = "Already Connected."
                }
                if (ret !== Core.OK)
                {
                    message.visible = true;
                }
            }
        }
        MessageDialog{
            id: message
        }
    }
    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Resource")
        }
        TabButton {
            text: qsTr("Configuration")
        }
    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
