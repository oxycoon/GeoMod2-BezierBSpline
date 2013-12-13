import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

import MyCustomQuick 1.0

Item {
  id: root

  signal moveFw
  signal moveBw
  signal moveLeft
  signal moveRight

  Item {
    id: view_root
    anchors.fill: parent

    Item {
      id: singleview
      anchors.fill: parent
      visible: false

      GLSceneRenderer { id: singleview_renderer; anchors.fill: parent }

//      Rectangle {
//        anchors.fill: parent
//        color: "red"
//      }

      MouseArea {
        anchors.fill: parent
        onDoubleClicked: view_root.state = ""
      }
    }

    SplitView {
      id: multiview
      anchors.fill: parent
      orientation: Qt.Vertical

      onVisibleChanged: {

        projection_view_renderer.forceRender()
        top_view_renderer.forceRender()
        front_view_renderer.forceRender()
        side_view_renderer.forceRender()
      }

      SplitView {
        orientation: Qt.Horizontal
        height: 200

        Item {
          id: top_left
          Layout.fillWidth: true

          GLSceneRenderer {
            id: top_view_renderer
            anchors.fill: parent
            name: "top_camera"
          }

          MouseArea {
            anchors.fill:parent
            focus: true
            onDoubleClicked: view_root.state = "top_view"
          }
        }

        Item {
          id: top_right
          width: 200

          GLSceneRenderer {
            id: side_view_renderer
            anchors.fill: parent
            name: "side_camera"
          }

          MouseArea {
            anchors.fill:parent
            focus: true
            onDoubleClicked: view_root.state = "side_view"
          }

          onWidthChanged: bottom_right.width = width
        }
      }

      SplitView {
        orientation: Qt.Horizontal
        Layout.fillHeight: true

        Item {
          id: bottom_left
          Layout.fillWidth: true

          GLSceneRenderer {
            id: projection_view_renderer
            anchors.fill: parent
            focus: true
            name: "projection_camera"
            Keys.onPressed: {

              switch(event.key) {
              case Qt.Key_Up:     root.moveFw();    break;
              case Qt.Key_Down:   root.moveBw();    break;
              case Qt.Key_Left:   root.moveLeft();  break;
              case Qt.Key_Right:  root.moveRight(); break;
              }
            }
          }

          MouseArea {
            anchors.fill:parent
            focus: true
            onDoubleClicked: view_root.state = "projection_view"
          }
        }

        Item {
          id: bottom_right
          width: top_right.width

          GLSceneRenderer {
            id: front_view_renderer
            anchors.fill: parent
            name: "front_camera"
          }

          MouseArea {
            anchors.fill:parent
            focus: true
            onDoubleClicked: view_root.state = "front_view"
          }

          onWidthChanged: top_right.width = width
        }
      }
    }

    states: [
      State{
        name: "projection_view"
        PropertyChanges { target: multiview; visible: false }
        PropertyChanges { target: singleview; visible: true }
        PropertyChanges { target: singleview_renderer; name: "projection_camera" }
      },
      State{
        name: "top_view"
        PropertyChanges { target: multiview; visible: false }
        PropertyChanges { target: singleview; visible: true }
        PropertyChanges { target: singleview_renderer; name: "top_camera" }
      },
      State{
        name: "side_view"
        PropertyChanges { target: multiview; visible: false }
        PropertyChanges { target: singleview; visible: true }
        PropertyChanges { target: singleview_renderer; name: "side_camera" }
      },
      State{
        name: "front_view"
        PropertyChanges { target: multiview; visible: false }
        PropertyChanges { target: singleview; visible: true  }
        PropertyChanges { target: singleview_renderer;  name: "front_camera" }
      }
    ]
  }
}
