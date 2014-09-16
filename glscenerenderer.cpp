#include "glscenerenderer.h"

// local
#include "window.h"
#include "guiapplication.h"

// hidmanager
//#include <standardhidmanager.h>

// Qt
#include <QSGSimpleTextureNode>




class Texture : public QSGDynamicTexture {
public:
  Texture( const std::string& name ) { _tex.acquire(name); }

  void        acquireTexId( const std::string& name ) { _tex.acquire(name); }
  void        setSize( const QSize& size ) { _size = size; }

  // virtual fron QSGTexture


  int         textureId() const override { return _tex.getId(); }
  bool        hasAlphaChannel() const override { return true; }
  bool        hasMipmaps() const override { return false; }
  QSize       textureSize() const override { return _size; }

  void        bind() override { _tex.bind(); }

  bool        updateTexture() override { return true; }

private:
  GMlib::GL::Texture    _tex;
  QSize                 _size;

};








class GMlibRenderTextureNode : public QSGSimpleTextureNode {
public:
  GMlibRenderTextureNode(const std::string& name)
    : QSGSimpleTextureNode(), _texture {name} { setTexture(&_texture); }
  ~GMlibRenderTextureNode() { setTexture(nullptr); }


  void        setTexName( const std::string& name ) {  _texture.acquireTexId(name); }
  void        setTexSize( const QSize& size ) {  _texture.setSize(size);  }
  void        update() { _texture.updateTexture(); markDirty(QSGNode::DirtyGeometry); }

private:
  Texture           _texture;
};













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

//  qDebug() << "Force render! : " << _tex_name;
  emit signRenderGeometryChanged(_tex_name, boundingRect() );
}

QSGNode* GLSceneRenderer::updatePaintNode(QSGNode* old_node, QQuickItem::UpdatePaintNodeData*) {

  if (width() <= 0 || height() <= 0 || _tex_name.length() <= 0 | _paused) {

//    if( old_node ) delete old_node;
    delete old_node;
    return nullptr;
  }

  GMlibRenderTextureNode *node = static_cast<GMlibRenderTextureNode *>(old_node);
  if( !node )
    node = new GMlibRenderTextureNode(_tex_name.toStdString());

  const QRectF r = boundingRect();
  node->setRect( QRectF(r.left(), r.top()+r.height(), r.width(), -r.height() ) );

  if( _tex_size != r.toRect().size() ) {

////    qDebug() << "old: " << _tex_size << ", new: " << r.toRect().size() << "  --> " << _tex_name;
    _tex_size = r.toRect().size();
    node->setTexSize(_tex_size);
    emit signRenderGeometryChanged(_tex_name, r );
  }

  node->update();

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
