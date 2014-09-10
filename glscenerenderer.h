#include <opengl/gmtexture>

#include <QtQuick/QQuickItem>
#include <QtCore/QPropertyAnimation>

class GLSceneRenderer : public QQuickItem {
  Q_OBJECT
  Q_PROPERTY(QString name     READ getTexName WRITE setTexName)
  Q_PROPERTY(bool    paused   READ isPaused   WRITE setPaused)
public:
  GLSceneRenderer( QQuickItem *parent = 0 );

  const QString&        getTexName() const;
  void                  setTexName( const QString& tex_name );

  bool                  isPaused() const;
  void                  setPaused( bool paused );

  Q_INVOKABLE   void    forceRender();



signals:
  void                  signRenderGeometryChanged(const QString& name, const QRectF &geometry);

protected:
  // virtual from QQuickItem
  QSGNode*              updatePaintNode(QSGNode *node, UpdatePaintNodeData *data);
  void                  itemChange(ItemChange, const ItemChangeData &);
  void                  geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
  QSize                 _tex_size;
  GMlib::GL::Texture    _tex;
  QString               _tex_name;

  bool                  _paused;

};
