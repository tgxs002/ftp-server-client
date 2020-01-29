import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Item {
    id: item1
    width: 600
    height: 320
    property alias item1: item1
    property alias textInput: textInput
    property alias logout: logout
    property alias longin: longin
    property alias disconnect: disconnect
    property alias switch1: switch1
    property alias passwd: passwd
    property alias userName: userName
    property alias portInput: portInput
    property alias ipInput: ipInput
    property alias connect: connect

    ScrollView {
        id: scrollView
        width: parent.width
        height: parent.height

        GridLayout {
            id: columnLayout
            x: 20
            y: 20
            width: (item1.width - 40) * 1
            height: parent.height - 60
            columns: 2

            GroupBox {
                id: address
                height: 200
                transformOrigin: Item.Center
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                title: qsTr("Address")

                Grid {
                    id: grid
                    width: parent.width * 1
                    height: parent.height * 1
                    columns: 2
                    Text {
                        id: ip
                        text: qsTr("Server IP: ")
                        font.pixelSize: 18
                    }

                    TextInput {
                        id: ipInput
                        width: 200
                        height: 20
                        text: qsTr("127.0.0.1")
                        autoScroll: false
                        echoMode: TextInput.Normal
                        selectionColor: "#00801c"
                        font.pixelSize: 18
                        validator: RegExpValidator {
                            regExp: /[1-2]?[0-9]?[0-9]?[.][1-2]?[0-9]?[0-9][.][1-2]?[0-9]?[0-9][.][1-2]?[0-9]?[0-9]/
                        }
                    }

                    Text {
                        id: port
                        text: qsTr("Port:       ")
                        font.pixelSize: 18
                    }

                    TextInput {
                        id: portInput
                        width: 200
                        height: 20
                        text: qsTr("21")
                        autoScroll: false
                        font.pixelSize: 18
                    }
                }
            }

            ColumnLayout {
                id: columnLayout1
                y: 16
                width: 100
                height: 77
                spacing: 0

                Button {
                    id: connect
                    width: 82
                    height: 30
                    text: qsTr("Connect")
                }

                Button {
                    id: disconnect
                    height: 30
                    text: qsTr("disconnect")
                }
            }

            GroupBox {
                id: login
                width: 200
                height: 200
                Layout.fillHeight: false
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                title: qsTr("Login")

                Grid {
                    id: grid1
                    anchors.fill: parent
                    columns: 2

                    Text {
                        id: user
                        text: qsTr("User name: ")
                        font.pixelSize: 18
                    }

                    TextInput {
                        id: userName
                        width: 200
                        height: 20
                        text: qsTr("anonymous")
                        autoScroll: false
                        font.pixelSize: 18
                    }

                    Text {
                        id: pass
                        text: qsTr("Password:   ")
                        font.pixelSize: 18
                    }

                    TextInput {
                        id: passwd
                        width: 200
                        height: 20
                        text: qsTr("")
                        autoScroll: false
                        enabled: true
                        passwordCharacter: "~"
                        echoMode: TextInput.Password
                        font.pixelSize: 18
                    }
                }
            }

            ColumnLayout {
                id: columnLayout2
                width: 100
                height: 100

                Button {
                    id: longin
                    height: 30
                    text: qsTr("login")
                }

                Button {
                    id: logout
                    height: 30
                    text: qsTr("logout")
                }
            }

            GroupBox {
                id: mode
                width: 200
                height: 120
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                title: qsTr("Mode")

                Grid {
                    id: grid2
                    width: mode.width
                    height: mode.height
                    spacing: 10
                    transformOrigin: Item.Center
                    columns: 2

                    Switch {
                        id: switch1
                        y: -10
                        text: qsTr("Passive")
                    }

                    Item {
                        id: item2
                        width: 200
                        height: switch1.height
                    }

                    Text {
                        id: text1
                        text: qsTr("Path: ")
                        font.pixelSize: 18
                    }

                    TextInput {
                        id: textInput
                        width: 300
                        height: 20
                        text: qsTr("/tmp")
                        autoScroll: false
                        selectionColor: "#008068"
                        font.pixelSize: 18
                    }
                }
            }

            Item {
                id: item3
                width: 100
                height: 200
            }

            Item {
                id: item4
                width: 200
                height: 30
            }
        }
    }
}
