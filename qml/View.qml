import QtQuick 2.2
import QtQuick.Controls 1.1

import MyCustomQuick 1.0


Item {
  id: view

  property string name : ""
  property bool   resizing : false
  property bool   has_been_resized : false

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
    paused: false
  }

  Label {
   anchors.top: parent.top
   anchors.left: parent.left
   anchors.margins: 5
   text: view.name
  }

  onResizingChanged: {
    has_been_resized = has_been_resized | resizing


    console.debug(resizing + " : " + has_been_resized )

    if( !resizing && has_been_resized ) {
      has_been_resized = false
      forceRender()
    }

    console.debug(resizing + " -> " + has_been_resized )
  }

  Component.onCompleted: {
    view.forceRender.connect(renderer.forceRender)
  }

  onVisibleChanged: forceRender()
}
