import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1

import MyCustomQuick 1.0

import "qrc:/qml"

Item {

  MySplitView2 {
    anchors.fill: parent
    orientation: Qt.Horizontal

    id: layout



    MySplitView2 {
      orientation: Qt.Vertical
      View {
          Behavior on opacity  { NumberAnimation  {} }

          visible: true
          width: 40
          name: "projection_camera"
          Layout.fillWidth: true
      }

      View {
          Behavior on opacity  { NumberAnimation  {} }

          visible: true
          width: 40
          name: "front_camera"
          Layout.fillWidth: true
      }
    }


    MySplitView2{

      orientation: Qt.Vertical

      View {
          Behavior on opacity  { NumberAnimation  {} }

          visible: true
          name: "top_camera"
          Layout.fillWidth: true
      }

      View {
          Behavior on opacity  { NumberAnimation  {} }

          visible: true
          name: "side_camera"
          Layout.fillWidth: true
      }
    }
  }
}
