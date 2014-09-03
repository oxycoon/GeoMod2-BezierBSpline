import QtQuick 2.0
import QtQuick.Layouts 1.0

import "qrc:/qml" as Q

Item {

  property string left_camera: ""
  property string right_camera: ""


  RowLayout {
    anchors.fill: parent

    Rectangle {
      width: 50

      color: "blue"
    }

    Rectangle {
      Layout.fillWidth: true

      color: "red"
    }


//    Q.NewView {
//      width: 50
//      name: left_camera
//    }

//    Q.NewView {
//      Layout.fillWidth: true
//      name: right_camera
//    }
  }
}
