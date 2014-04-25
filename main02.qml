import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1

import MyCustomQuick 1.0

import "qrc:/qml" as Q

Item {
  id: view_root

  signal moveFw
  signal moveBw
  signal moveLeft
  signal moveRight

//  onHeightChanged: multiview.adjustSizeToRatio()

  Q.View {
    id: singleview
    anchors.fill: parent
    visible: false

    MouseArea {
      anchors.fill: parent
      onDoubleClicked: view_root.state = ""
    }
  }

  Q.ViewSet {
    id: multiview
    anchors.fill: parent
    orientation: Qt.Vertical

    property double h_ratio : 0.5
    property double v_ratio : 0.5
    property bool   has_been_resized : false

//    onResizingChanged: {

//      has_been_resized = !resizing | has_been_resized
//      if( has_been_resized && !resizing ) {

//        computeRatio()
//        has_been_resized = false
//      }
//    }

//    onHeightChanged: multiview.adjustSizeToRatio()

//    function computeRatio() {

//      h_ratio = mv_top.height / (mv_top.height+mv_bottom.height)
//      v_ratio = mv_top.width / (mv_top.width+mv_bottom.width)
//    }

//    function adjustSizeToRatio() {

//      console.debug("multiview has " + multiview.children.length + " children")

//      console.debug("mv_top: " + mv_top )
//      console.debug("top_view_renderer: " + top_view_renderer )

//      for( var i = 0; i < multiview.children.length; ++i ) console.debug("bah " + i + ": " + multiview.children[i])

//      mv_top.height = multiview.height * h_ratio
//      mv_top.width  = multiview.width  * v_ratio
//    }



    SplitView {
      id: mv_top
      orientation: Qt.Horizontal
      height: 200


      Q.View {
        id:top_view_renderer
        name: "top_camera"
        resizing: mv_top.resizing

        Layout.fillWidth: true

        MouseArea {
          anchors.fill:parent
          focus: true
          onDoubleClicked: view_root.state = "top_view"
        }

        Component.onCompleted: resizing = Qt.binding( function() { return multiview.resizing | mv_bottom.resizing | mv_top.resizing } )
      }

      Q.View {
        id:side_view_renderer
        name: "side_camera"

        width: 200

        MouseArea {
          anchors.fill:parent
          focus: true
          onDoubleClicked: view_root.state = "side_view"
        }

        Component.onCompleted: resizing = Qt.binding( function() { return multiview.resizing | mv_bottom.resizing | mv_top.resizing } )
        onWidthChanged: front_view_renderer.width = width
      }
    }

    SplitView {
      id: mv_bottom
      orientation: Qt.Horizontal
      Layout.fillHeight: true

      Q.View {
        id:projection_view_renderer
        name: "projection_camera"
        resizing: mv_bottom.resizing

        Layout.fillWidth: true

        MouseArea {
          anchors.fill:parent
          focus: true
          onDoubleClicked: view_root.state = "projection_view"
        }

        Keys.onPressed: {

          switch(event.key) {
          case Qt.Key_Up:     view_root.moveFw();    break;
          case Qt.Key_Down:   view_root.moveBw();    break;
          case Qt.Key_Left:   view_root.moveLeft();  break;
          case Qt.Key_Right:  view_root.moveRight(); break;
          }
        }

        Component.onCompleted: resizing = Qt.binding( function() { return multiview.resizing | mv_bottom.resizing | mv_top.resizing } )
      }

      Q.View {
        id:front_view_renderer
        name: "front_camera"
        resizing: mv_bottom.resizing

        width: side_view_renderer.width

        MouseArea {
          anchors.fill:parent
          focus: true
          onDoubleClicked: view_root.state = "front_view"
        }

        Component.onCompleted: resizing = Qt.binding( function() { return multiview.resizing | mv_bottom.resizing | mv_top.resizing } )
        onWidthChanged: side_view_renderer.width = width
      }
    }
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
