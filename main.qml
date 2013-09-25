import QtQuick 2.1
import MyCustomQuick 1.0

Item {
  id: root

  signal moveFw
  signal moveBw
  signal moveLeft
  signal moveRight

  GLSceneRenderer {
    anchors.fill: parent
    focus: true
    Keys.onPressed: {

      switch(event.key) {
      case Qt.Key_Up:     root.moveFw();    break;
      case Qt.Key_Down:   root.moveBw();    break;
      case Qt.Key_Left:   root.moveLeft();  break;
      case Qt.Key_Right:  root.moveRight(); break;
      }
    }
  }

}
