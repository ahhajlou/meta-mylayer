import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        width: 640
        height: 480
        color: "#f0f0f0"

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "Click in the text field below and start typing"
                font.pixelSize: 18
                color: "#333"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Simple TextField - handles keyboard automatically
            TextField {
                id: textInput
                width: 400
                placeholderText: "Type here..."
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Multi-line TextArea for longer text
            Rectangle {
                width: 400
                height: 200
                color: "white"
                border.color: "#ccc"
                border.width: 2
                radius: 5
                anchors.horizontalCenter: parent.horizontalCenter

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 5

                    TextArea {
                        id: textDisplay
                        placeholderText: "Or type multiple lines here..."
                        wrapMode: TextArea.Wrap
                        font.pixelSize: 16
                        Component.onCompleted: {
                            forceActiveFocus()  // Ensure it gets focus
                        }
                    }
                }
            }

            Button {
                id: clearButton
                text: "Clear Both"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Connections {
            target: clearButton
            onClicked: {
                textInput.text = ""
                textDisplay.text = ""
                textDisplay.pressed()
            }
        }
    }
}
