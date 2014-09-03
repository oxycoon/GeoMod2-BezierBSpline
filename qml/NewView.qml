import QtQuick 2.0
import QtQuick.Controls 1.0

import MyCustomQuick 1.0

Item {
  id: view

  property string name : ""

  GLSceneRenderer {
    id: renderer
    anchors.fill: parent
    name: view.name
    paused: false
  }

}
