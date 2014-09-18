#include "glscenerenderer2.h"
#include "window.h"

// Qt
#include <QQuickWindow>

// stl
#include <iostream>







GLSceneRenderer2::GLSceneRenderer2() : _renderer{nullptr}, _name{}, _paused{true} {

  setFlag(ItemHasContents);
  setSmooth(false);
  connect( this, &QQuickItem::windowChanged, this, &GLSceneRenderer2::handleWindowChanged );
}

const
QString&  GLSceneRenderer2::getTexName() const {

  return _name;
}

void
GLSceneRenderer2::setTexName(const QString& name) {

  _name = name;
//  if(_renderer)
//    _renderer->setTexName(_name.toStdString());
}

bool
GLSceneRenderer2::isPaused() const {

  return _paused;
}

void GLSceneRenderer2::setPaused(bool paused) {

  _paused = paused;
}

void
GLSceneRenderer2::sync() {


  if(!_renderer && _name.length() > 0) {
    _renderer = std::unique_ptr<Private::Renderer>(new Private::Renderer(_name.toStdString()));
    connect( window(), &QQuickWindow::beforeRendering, _renderer.get(), &Private::Renderer::paint, Qt::DirectConnection );
  }

  if( !_renderer )
    return;

  QRectF r = mapRectToScene(boundingRect());
  QRectF cr(r.left(),window()->height()-r.bottom(), r.width(), r.height());
//  _renderer->setViewport(r.left(),window()->height()-r.bottom(), r.width(), r.height() );
  _renderer->setViewport(cr);

  emit signViewportChanged(_name,r);
}

void
GLSceneRenderer2::cleanup() {

  std::cout << "Cleanup" << std::endl;
}

void
GLSceneRenderer2::handleWindowChanged(QQuickWindow* window) {

  if( !window ) return;

  Window *w = dynamic_cast<Window*>(window);
  if( !w ) return;

  connect( w, &QQuickWindow::beforeSynchronizing, this, &GLSceneRenderer2::sync );
  connect( w, &QQuickWindow::sceneGraphInvalidated, this, &GLSceneRenderer2::cleanup );
  connect( w, &Window::signFrameReady, this, &GLSceneRenderer2::update );
  connect( this, &GLSceneRenderer2::signViewportChanged, w, &Window::signGuiViewportChanged );
}
