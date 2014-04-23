import QtQuick 2.2
import QtQuick.Controls 1.1

import MyCustomQuick 1.0


Item {
  id: view

  property string name : ""
  property bool   resizing : false

  signal forceRender

  Rectangle {
    id: bah
    color: "gray"
    anchors.fill: parent
    visible: view.resizing
  }

  GLSceneRenderer {
    id: renderer
    anchors.fill: parent
    name: view.name
    visible: !view.resizing
    paused: resizing
  }

  Label {
   anchors.top: parent.top
   anchors.left: parent.left
   anchors.margins: 5
   text: view.name
  }

  Component.onCompleted: {
    view.forceRender.connect(renderer.forceRender)
  }
}
