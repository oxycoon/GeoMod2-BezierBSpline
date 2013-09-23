#include <opengl/gmTexture>

#include <QtQuick/QQuickItem>
#include <QtCore/QPropertyAnimation>

class GLSceneRenderer : public QQuickItem {
  Q_OBJECT

public:
  GLSceneRenderer( QQuickItem *parent = 0 );

signals:
  void                  signRenderGeometryChanged(const QRectF &geometry);

protected:
  // virtual from QQuickItem
  QSGNode*              updatePaintNode(QSGNode *node, UpdatePaintNodeData *data);
  void                  itemChange(ItemChange, const ItemChangeData &);
  void                  geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
  GMlib::GL::Texture    _tex;
  QSize                 _tex_size;
};
