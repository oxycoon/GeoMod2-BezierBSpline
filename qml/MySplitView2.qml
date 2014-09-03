/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Private 1.0 as Private
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1


Item {
    id: root

    /*!
        \qmlproperty enumeration SplitView::orientation

        This property holds the orientation of the SplitView.
        The value can be either \c Qt.Horizontal or \c Qt.Vertical.
        The default value is \c Qt.Horizontal.
    */
    property int orientation: Qt.Horizontal

    /*!
        This property holds the delegate that will be instantiated between each
        child item. Inside the delegate the following properties are available:

        \table
            \row \li readonly property bool styleData.index \li Specifies the index of the splitter handle. The handle
                                                         between the first and the second item will get index 0,
                                                         the next handle index 1 etc.
            \row \li readonly property bool styleData.hovered \li The handle is being hovered.
            \row \li readonly property bool styleData.pressed \li The handle is being pressed.
            \row \li readonly property bool styleData.resizing \li The handle is being dragged.
        \endtable

*/
    property Component handleDelegate: Rectangle {
        width: 1
        height: 1
        color: Qt.darker(pal.window, 1.5)
    }

    /*!
        This propery is \c true when the user is resizing any of the items by
        dragging on the splitter handles.
    */
    property bool resizing: false

    /*! \internal */
    default property alias __contents: contents.data
    /*! \internal */
    property alias __items: splitterItems.children
    /*! \internal */
    property alias __handles: splitterHandles.children

    clip: true
    Component.onCompleted: d.init()
    onWidthChanged: d.updateLayout()
    onHeightChanged: d.updateLayout()
    onOrientationChanged: d.changeOrientation()

    SystemPalette { id: pal }

    QtObject {
        id: d
        property bool horizontal: orientation == Qt.Horizontal
        readonly property string minimum: horizontal ? "minimumWidth" : "minimumHeight"
        readonly property string maximum: horizontal ? "maximumWidth" : "maximumHeight"
        readonly property string otherMinimum: horizontal ? "minimumHeight" : "minimumWidth"
        readonly property string otherMaximum: horizontal ? "maximumHeight" : "maximumWidth"
        readonly property string offset: horizontal ? "x" : "y"
        readonly property string otherOffset: horizontal ? "y" : "x"
        readonly property string size: horizontal ? "width" : "height"
        readonly property string otherSize: horizontal ? "height" : "width"
        readonly property string implicitSize: horizontal ? "implicitWidth" : "implicitHeight"
        readonly property string implicitOtherSize: horizontal ? "implicitHeight" : "implicitWidth"

        property int fillIndex: -1
        property bool updateLayoutGuard: true

        property bool validHandles : false


        function init()
        {

          console.debug("d.init()")
          console.debug("  root.size: " + root[d.size])
          console.debug("  root.implicitSize: " + root[d.implicitSize])
          console.debug("  items+handles size: " + splitterItems[d.size] + splitterHandles[d.size])
          var i
            for (i=0; i<__contents.length; ++i) {
                var item = __contents[i];
                if (!item.hasOwnProperty("x"))
                    continue

                if (splitterItems.children.length > 0)
                    handleLoader.createObject(splitterHandles, {"__handleIndex":splitterItems.children.length - 1})
                item.parent = splitterItems
                i-- // item was removed from list

                // should match disconnections in Component.onDestruction
//                item.widthChanged.connect(d.updateLayout)
//                item.heightChanged.connect(d.updateLayout)
            }
          console.debug("  No. items: " + __items.length)
          console.debug("  No. handles: " + __handles.length)

            for( i = 0; i < __handles.length; ++i)
              __handles[i].relativeOffset = i === 0 ? 1.0 / __items.length : __handles[i-1].relativeOffset + 1.0 / __items.length


            d.calculateImplicitSize()
            d.updateLayoutGuard = false
//            d.updateFillIndex()

            d.updateLayout()
        }



//        function updateFillIndex()
//        {
//            if (lastItem.visible !== root.visible)
//                return
//            var policy = (root.orientation === Qt.Horizontal) ? "fillWidth" : "fillHeight"
//            for (var i=0; i<__items.length-1; ++i) {
//                if (__items[i].Layout[policy] === true)
//                    break;
//            }

//            d.fillIndex = i
//            d.updateLayout()
//        }

        function changeOrientation()
        {
            if (__items.length == 0)
                return;
            d.updateLayoutGuard = true

            // Swap width/height for items and handles:
            for (var i=0; i<__items.length; ++i) {
                var item = __items[i]
                var tmp = item.x
                item.x = item.y
                item.y = tmp
                tmp = item.width
                item.width = item.height
                item.height = tmp

                var handle = __handles[i]
                if (handle) {
                    tmp = handle.x
                    handle.x = handle.y
                    handle.y = handle.x
                    tmp = handle.width
                    handle.width = handle.height
                    handle.height = tmp
                }
            }

            // Change d.horizontal explicit, since the binding will change too late:
            d.horizontal = orientation == Qt.Horizontal
            d.updateLayoutGuard = false
            d.updateFillIndex()
        }

        function calculateImplicitSize()
        {
            var implicitSize = 0
            var implicitOtherSize = 0

            for (var i=0; i<__items.length; ++i) {
                var item = __items[i];
                implicitSize += clampedMinMax(item[d.size], item.Layout[minimum], item.Layout[maximum])
                var os = clampedMinMax(item[otherSize], item.Layout[otherMinimum], item.Layout[otherMaximum])
                implicitOtherSize = Math.max(implicitOtherSize, os)

                var handle = __handles[i]
                if (handle)
                    implicitSize += handle[d.size]
            }

            root[d.implicitSize] = implicitSize
            root[d.implicitOtherSize] = implicitOtherSize
        }

        function clampedMinMax(value, minimum, maximum)
        {
            if (value < minimum)
                value = minimum
            if (value > maximum)
                value = maximum
            return value
        }

        function accumulatedSize(firstIndex, lastIndex, includeFillItemMinimum)
        {
            // Go through items and handles, and
            // calculate their acummulated width.
            var w = 0
            for (var i=firstIndex; i<lastIndex; ++i) {

                var item = __items[i]
                if (item.visible || i == d.fillIndex) {
                    if (i !== d.fillIndex)
                        w += item[d.size];
                    else if (includeFillItemMinimum && item.Layout[minimum] !== undefined)
                        w += item.Layout[minimum]
                }

                var handle = __handles[i]
                if (handle && handle.visible)
                    w += handle[d.size]
            }
            return w
        }

        function updateLayout()
        {

          console.debug("UPDATE LAYOUTS")

            // This function will reposition both handles and
            // items according to the their width/height:
            if (__items.length === 0)
                return;
            if (!lastItem.visible)
                return;
            if (d.updateLayoutGuard === true)
                return
            d.updateLayoutGuard = true

          console.debug("UPDATE LAYOUTS - PAST GUARDS")
          console.debug("No. Handles: " + __handles.length )
          console.debug("No. Items: " + __items.length )
          for( var i2 = 0; i2 < __handles.length; ++i2 )
            console.debug("  handle[" + i2 + "] rel offset: " + __handles[i2].relativeOffset)

            // Ensure all items within their min/max:
//            for (var i=0; i<__items.length; ++i) {
//                if (i !== d.fillIndex) {
//                    var item = __items[i];
//                    var clampedSize = clampedMinMax(item[d.size], item.Layout[d.minimum], item.Layout[d.maximum])
//                    if (clampedSize != item[d.size])
//                        item[d.size] = clampedSize
//                }
//            }

            // Set size of fillItem to remaining available space.
            // Special case: If SplitView size is zero, we leave fillItem with the size
            // it already got, and assume that SplitView ends up with implicit size as size:
//            if (root[d.size] != 0) {
//                var fillItem = __items[fillIndex]
//                var superfluous = root[d.size] - d.accumulatedSize(0, __items.length, false)
//                var s = Math.max(superfluous, fillItem.Layout[minimum])
//                s = Math.min(s, fillItem.Layout[maximum])
//                fillItem[d.size] = s
//            }

//            // Position items and handles according to their width:
//            var lastVisibleItem, lastVisibleHandle, handle
//            for (i=0; i<__items.length; ++i) {
//                // Position item to the right of the previous visible handle:
//                item = __items[i];
//                if (item.visible || i == d.fillIndex) {
//                    item[d.offset] = lastVisibleHandle ? lastVisibleHandle[d.offset] + lastVisibleHandle[d.size] : 0
//                    item[d.otherOffset] = 0
//                    item[d.otherSize] = clampedMinMax(root[otherSize], item.Layout[otherMinimum], item.Layout[otherMaximum])
//                    lastVisibleItem = item
//                }

//                handle = __handles[i]
//                if (handle && handle.visible) {
//                    handle[d.offset] = lastVisibleItem[d.offset] + Math.max(0, lastVisibleItem[d.size])
//                    handle[d.otherOffset] = 0
//                    handle[d.otherSize] = root[d.otherSize]
//                    lastVisibleHandle = handle
//                }
//            }

            // Position items and handles according to the relative handle positions
            var lastVisibleItem, lastVisibleHandle, handle, item
            for (var i=0; i<__items.length; ++i) {

                // Position item to the right of the previous visible handle:
                handle = __handles[i]
                if (handle && handle.visible) {
//                    handle[d.offset] = lastVisibleItem[d.offset] + Math.max(0, lastVisibleItem[d.size])
                    handle[d.offset] = handle.relativeOffset * root[d.size]
                    console.debug("HANDLE OFFSET[" + i + "]: " + handle[d.offset])
                    handle[d.otherOffset] = 0
                    handle[d.otherSize] = root[d.otherSize]
                }

                item = __items[i];
                if( item.visible || i == d.fillIndex) {
                  item[d.offset] = lastVisibleHandle ? lastVisibleHandle[d.offset] + lastVisibleHandle[d.size] : 0
                  item[d.otherOffset] = 0
                  item[d.otherSize] = root[otherSize]

                  if(handle && handle.visible)
                    item[d.size] = lastVisibleHandle ? handle[d.offset] - lastVisibleHandle[d.offset] : handle[d.offset]
                  else
                    item[d.size] = root[d.size]
                  lastVisibleItem = item
                }
                lastVisibleHandle = handle

            }

            d.updateLayoutGuard = false
        }
    }

    Component {
        id: handleLoader
        Loader {
            id: itemHandle

            property int __handleIndex: -1
            property QtObject styleData: QtObject {
                readonly property int index: __handleIndex
                readonly property alias hovered: mouseArea.containsMouse
                readonly property alias pressed: mouseArea.pressed
                readonly property bool resizing: mouseArea.drag.active
                onResizingChanged: root.resizing = resizing
            }
            property real relativeOffset: 0.33
            property bool resizeLeftItem: (d.fillIndex > __handleIndex)
            visible: __items[__handleIndex].visible // __items[__handleIndex + (resizeLeftItem ? 0 : 1)].visible
            sourceComponent: handleDelegate
            onWidthChanged: d.updateLayout()
            onHeightChanged: d.updateLayout()
            onXChanged: { moveHandle(); d.updateLayout() }
            onYChanged: { moveHandle(); d.updateLayout() }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                property real defaultMargin: Private.Settings.hasTouchScreen ? Screen.pixelDensity * 3.5 : 2
                anchors.leftMargin: (parent.width <= 1) ? -defaultMargin : 0
                anchors.rightMargin: (parent.width <= 1) ? -defaultMargin : 0
                anchors.topMargin: (parent.height <= 1) ? -defaultMargin : 0
                anchors.bottomMargin: (parent.height <= 1) ? -defaultMargin : 0
                hoverEnabled: true
                drag.threshold: 0
                drag.target: parent
                drag.axis: root.orientation === Qt.Horizontal ? Drag.XAxis : Drag.YAxis
                cursorShape: root.orientation === Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor
            }

            function moveHandle() {
                // Moving the handle means resizing an item. Which one,
                // left or right, depends on where the fillItem is.
                // 'updateLayout' will be overridden in case new width violates max/min.
                // 'updateLayout' will be triggered when an item changes width.
                if (d.updateLayoutGuard)
                    return

                console.debug("Parent size: " + root[d.size])
                console.debug(" handle offset: " + this[d.offset])
                console.debug(" relative: " + this[d.offset]/root[d.size])
                console.debug(" index: " + __handleIndex)

                relativeOffset = this[d.offset]/ Math.max(root[d.size],root[d.implicitSize])
            }

        }
    }

    Item {
        id: contents
        visible: false
        anchors.fill: parent
    }
    Item {
        id: splitterItems
        anchors.fill: parent
    }
    Item {
        id: splitterHandles
        anchors.fill: parent
    }

    Item {
        id: lastItem
        onVisibleChanged: d.updateFillIndex()
    }

    Component.onDestruction: {
        for (var i=0; i<splitterItems.children.length; ++i) {
            var item = splitterItems.children[i];

            // should match connections in init()
//            item.widthChanged.disconnect(d.updateLayout)
//            item.heightChanged.disconnect(d.updateLayout)
        }
    }
}
