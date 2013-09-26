#include "guiapplication.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QOpenGLContext>
#include <QQuickItem>
#include <QDebug>

GuiApplication::GuiApplication(int argc, char *argv[]) :
  QGuiApplication(argc, argv)
{
  _window = new Window;

  connect( _window, &Window::sceneGraphInitialized,
           this,    &GuiApplication::onSGInit );

  _window->show();
}

void GuiApplication::onSGInit() {

  qDebug() << "SG initialized";

  QOpenGLContext *ctx = _window->openglContext();
  if( !ctx->isValid() ) {

    qCritical() << "OpenGLContext not valid";
    exit(1);
  }
  qDebug() << "Initialized GLformat: " << ctx->format();

  // Init GMlibWrapper
  _gmlib = new GMlibWrapper( ctx, _window->size() );
  connect( _gmlib,  &GMlibWrapper::signFrameReady,
           _window, &Window::signFrameReady );
  connect( _window, &Window::signSceneRenderGeometryChanged,
           _gmlib,  &GMlibWrapper::changeRenderGeometry );

  _gmlib->start();




  QQuickItem *root_item = _window->rootObject();
  connect( root_item, SIGNAL(moveFw()),     _gmlib, SLOT(moveObjFw()) );
  connect( root_item, SIGNAL(moveBw()),     _gmlib, SLOT(moveObjBw()) );
  connect( root_item, SIGNAL(moveLeft()),   _gmlib, SLOT(moveObjLeft()) );
  connect( root_item, SIGNAL(moveRight()),  _gmlib, SLOT(moveObjRight()) );

}
