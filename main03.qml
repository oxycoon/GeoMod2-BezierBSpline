import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1

import MyCustomQuick 1.0

import "qrc:/qml"

Item {

//  MySplitView2 {
//    anchors.fill: parent
//    orientation: Qt.Horizontal
//    View { visible: true; name: "projection_camera" }
//    View { visible: true; name: "front_camera" }
//  }

  MySplitView2 {
    anchors.fill: parent
    orientation: Qt.Horizontal

    MySplitView2 {
      orientation: Qt.Vertical
      View { visible: true; name: "projection_camera" }
      View { visible: true; name: "front_camera" }
    }

    MySplitView2{
      orientation: Qt.Vertical
      View { visible: true; name: "top_camera" }
      View { visible: true; name: "side_camera" }
    }
  }
}
