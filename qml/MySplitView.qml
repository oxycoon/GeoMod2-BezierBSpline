import QtQuick 2.2
import QtQuick.Controls 1.1


Item {

  default property alias _contents: splitview.__contents
  property alias orientation : splitview.orientation
  readonly property alias items :  splitview.__items
  readonly property alias handles:  splitview.__handles


  property var __handle_positions : []

  SplitView {
    id: splitview

    anchors.fill: parent

    Component.onCompleted: {

      d.init()
    }

    onResizingChanged: {

      if( resizing  === false ) {
        console.debug("Touched and draged handlebars!")
        d.computeHandlePositions()
      }
    }

  }


  QtObject {
    id: d

    function init() {

      d.computeHandlePositions()
    }


    function computeHandlePositions() {


      console.debug("Splitview orientation: " + splitview.orientation)

      var no_handles      = handles.length
      console.debug("Number of handles: " + no_handles)

      var horizontal = splitview.orientation === Qt.Horizontal
      var sv_impl_size = horizontal ? splitview.implicitWidth : splitview.implicitHeight
      console.debug("Implicite size: " + sv_impl_size )

      console.debug("Splitview (2) implicitWidth: " + splitview.implicitWidth )
      console.debug("Splitview (2) implicitHeight: " + splitview.implicitHeight )

      var myarray = []
      for( var i = 0; i < handles.length; ++i ) {
        var handle = handles[i]
        var item = items[i]

        myarray.push(item.width/sv_impl_size)

      }

      __handle_positions = myarray

      for( var i = 0; i < __handle_positions.length; ++i )
        console.debug( "handle " + i + " relative position: " + __handle_positions[i] )

    }
  }
}




