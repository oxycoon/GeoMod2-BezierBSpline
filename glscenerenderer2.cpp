#include "glscenerenderer2.h"

// Qt
#include <QQuickWindow>


// stl
#include <iostream>







GLSceneRenderer2::GLSceneRenderer2() : _renderer{nullptr}, _tex_name{}, _paused{true} {

  connect( this, &QQuickItem::windowChanged, this, &GLSceneRenderer2::handleWindowChanged );
}

const QString&  GLSceneRenderer2::getTexName() const {

  return _tex_name;
}

void GLSceneRenderer2::setTexName(const QString& tex_name) {

  _tex_name = tex_name;
  if(_renderer)
    _renderer->setTexName(tex_name.toStdString());
}

bool GLSceneRenderer2::isPaused() const {

  return _paused;
}

void GLSceneRenderer2::setPaused(bool paused) {

  _paused = paused;
}

void
GLSceneRenderer2::sync() {

  if(!_renderer && _tex_name.length() > 0) {
    _renderer = std::unique_ptr<Private::Renderer>(new Private::Renderer(_tex_name.toStdString()));
    connect( window(), &QQuickWindow::beforeRendering, _renderer.get(), &Private::Renderer::paint, Qt::DirectConnection );
  }

  if( !_renderer )
    return;

  _renderer->setViewport( mapRectToScene(boundingRect()) );
  qDebug() << "Sync: " << _tex_name << ", bb: " << boundingRect();
}

void
GLSceneRenderer2::cleanup() {

  std::cout << "Cleanup" << std::endl;
}

void
GLSceneRenderer2::handleWindowChanged(QQuickWindow* window) {

  if( !window ) return;

  connect( window, &QQuickWindow::beforeSynchronizing, this, &GLSceneRenderer2::sync );
  connect( window, &QQuickWindow::sceneGraphInvalidated, this, &GLSceneRenderer2::cleanup );
//  window->setClearBeforeRendering(false);

}
