import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1

import MyCustomQuick 1.0

import "qrc:/qml"

Item {
  id: root

  View { id: singleview; anchors.fill: parent; visible: false; name: ""; onDoubleClicked: root.state = "" }

  RelativeSplitView {
    id: multiview
    anchors.fill: parent
    orientation: Qt.Vertical

    RelativeSplitView {
      id: sp1
      orientation: Qt.Horizontal
      View { visible: true; name: "top_camera";       onDoubleClicked: root.state = "top_view"; Component.onCompleted: resizing = Qt.binding(d.viewsChanging) }
      View { visible: true; name: "side_camera";      onDoubleClicked: root.state = "side_view"; Component.onCompleted: resizing = Qt.binding(d.viewsChanging) }
    }

    RelativeSplitView {
      id: sp2
      orientation: Qt.Horizontal
      View { visible: true; name: "projection_camera"; onDoubleClicked: root.state = "projection_view"; Component.onCompleted: resizing = Qt.binding(d.viewsChanging) }
      View { visible: true; name: "front_camera";      onDoubleClicked: root.state = "front_view"; Component.onCompleted: resizing = Qt.binding(d.viewsChanging) }
    }
  }

  QtObject {
    id: d

    function viewsChanging() { return false }// return multiview.resizing || sp1.resizing || sp2.resizing || root.resizing }
  }

  states: [
    State{
      name: "projection_view"
      PropertyChanges { target: multiview; visible: false }
      PropertyChanges { target: singleview; visible: true; name: "projection_camera" }
    },
    State{
      name: "top_view"
      PropertyChanges { target: multiview; visible: false }
      PropertyChanges { target: singleview; visible: true; name: "top_camera" }
    },
    State{
      name: "side_view"
      PropertyChanges { target: multiview; visible: false }
      PropertyChanges { target: singleview; visible: true; name: "side_camera" }
    },
    State{
      name: "front_view"
      PropertyChanges { target: multiview; visible: false }
      PropertyChanges { target: singleview; visible: true; name: "front_camera" }
    }
  ]

}
