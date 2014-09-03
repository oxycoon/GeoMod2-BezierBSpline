#include "glscenerenderer.h"

// local
#include "window.h"

// Qt
#include <QSGSimpleTextureNode>

GLSceneRenderer::GLSceneRenderer(QQuickItem *parent)
  : QQuickItem(parent),
    _tex_size(1,1),
    _paused(true)
{
  setFlag(ItemHasContents);
  setSmooth(false);
}

const QString&GLSceneRenderer::getTexName() const {

  return _tex_name;
}

void GLSceneRenderer::setTexName(const QString& tex_name) {

  _tex_name = tex_name;
  forceRender();
}

bool GLSceneRenderer::isPaused() const {

  return _paused;
}

void GLSceneRenderer::setPaused(bool paused) {

  _paused = paused;
}

void GLSceneRenderer::forceRender() {

  qDebug() << "Force render! : " << _tex_name;
  emit signRenderGeometryChanged(_tex_name, boundingRect() );
}

QSGNode* GLSceneRenderer::updatePaintNode(QSGNode* old_node, QQuickItem::UpdatePaintNodeData*) {

  if (width() <= 0 || height() <= 0 || _tex_name.length() <= 0 | _paused) {

    delete old_node;
    return 0x0;
  }

  QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>(old_node);
  if( !node ) {
    node = new QSGSimpleTextureNode;
    _tex.acquire(_tex_name.toStdString());
  }

  const QRectF r = boundingRect();
  node->setRect( QRectF(r.left(), r.top()+r.height(), r.width(), -r.height() ) );

  node->setTexture( window()->createTextureFromId( _tex.getId(), _tex_size ) );

  if( _tex_size != r.toRect().size() ) {

//    qDebug() << "old: " << _tex_size << ", new: " << r.toRect().size() << "  --> " << _tex_name;
    _tex_size = r.toRect().size();
    emit signRenderGeometryChanged(_tex_name, r );
  }

  return node;
}

void GLSceneRenderer::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) {

  if( change == QQuickItem::ItemSceneChange ) {

    if( value.window ) {

      Window *w = qobject_cast<Window*>(window());
      connect( w,     &Window::signFrameReady,
               this,  &GLSceneRenderer::update );
      connect( this,  &GLSceneRenderer::signRenderGeometryChanged,
               w,     &Window::signSceneRenderGeometryChanged );
    }
  }
}

void GLSceneRenderer::geometryChanged(const QRectF& new_geometry, const QRectF& /*oldGeometry*/) {

//  _tex_size = new_geometry.toRect().size();
//  emit signRenderGeometryChanged(_tex_name,new_geometry);
}



