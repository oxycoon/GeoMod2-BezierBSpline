import QtQuick 2.2
import QtQuick.Controls 1.1

import MyCustomQuick 1.0


Item {
  id: root

  property string name : ""
  property bool   resizing : false

  signal forceRender

  signal mousePressed( string view_name, int buttons, int modifiers, bool wasHeld, int x, int y )
  signal mouseReleased( string view_name, int buttons, int modifiers, bool wasHeld, int x, int y )
  signal mouseDoubleClicked( string view_name, int buttons, int modifiers, bool wasHeld, int x, int y )
  signal mousePositionChanged(string view_name, int buttons, int modifiers, bool wasHeld, int x, int y )

  signal keyPressed( string view_name, int key, int modifiers )
  signal keyReleased( string view_name, int key, int modifiers )

  Rectangle {
    id: bah
    color: "gray"
    anchors.fill: parent
    visible: false // root.resizing
  }

  FocusScope {
    id: scope
    anchors.fill: parent

    //    GLSceneRenderer {
    GLSceneRendererNoQSGTexture {
      id: renderer
      anchors.fill: parent
      name: root.name
      visible: !root.resizing
      paused: false

      Label {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 5
        text: root.name
        color: "white"
        font {
          bold: true
          weight: Font.Black
        }
      }
    }

    Keys.onPressed:  root.keyPressed(root.name,event.key,event.modifiers)
    Keys.onReleased: root.keyReleased(root.name,event.key,event.modifiers)


    MouseArea {
      anchors.fill: parent
      acceptedButtons: Qt.AllButtons
      onClicked: scope.focus = true
      onPressed:  root.mousePressed(root.name,mouse.buttons,mouse.modifiers,mouse.wasHeld,mouse.x,mouse.y)
      onReleased: root.mouseReleased(root.name,mouse.buttons,mouse.modifiers,mouse.wasHeld,mouse.x,mouse.y)
      onDoubleClicked: root.mouseDoubleClicked(root.name,mouse.buttons,mouse.modifiers,mouse.wasHeld,mouse.x,mouse.y)
      onPositionChanged: root.mousePositionChanged(root.name,mouse.buttons,mouse.modifiers,mouse.wasHeld,mouse.x,mouse.y)

//      onEntered: scope.focus = true
//      onExited: scope.focus = false
//      hoverEnabled: true
    }

  }

  onResizingChanged: {
    d.has_been_resized = d.has_been_resized | resizing

    if( !resizing && d.has_been_resized ) {
      d.has_been_resized = false
      root.forceRender()
    }
  }

  onVisibleChanged: forceRender()

  QtObject {
    id: d
    property bool   has_been_resized : false
  }

//  Component.onCompleted: root.forceRender.connect(renderer.forceRender)
}
