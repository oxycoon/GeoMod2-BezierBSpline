#include "guiapplication.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QOpenGLContext>
#include <QDebug>

GuiApplication::GuiApplication(int argc, char *argv[]) :
  QGuiApplication(argc, argv)
{
  _window = new Window;

//  connect( _window, &Window::sceneGraphInitialized,
//           this,    &GuiApplication::onSGInit,
//           Qt::DirectConnection );

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
  _gmlib = new GMlibWrapper( ctx, QSize(800,600) );
  connect( _gmlib,  &GMlibWrapper::signFrameReady,
           _window, &Window::signFrameReady );
  connect( _window, &Window::signSceneRenderGeometryChanged,
           _gmlib,  &GMlibWrapper::changeRenderGeometry );

  _gmlib->start();
}
