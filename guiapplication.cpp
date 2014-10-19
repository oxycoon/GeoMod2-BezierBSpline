#include "guiapplication.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QOpenGLContext>
#include <QQuickItem>
#include <QDebug>


// stl
#include <cassert>


std::unique_ptr<GuiApplication> GuiApplication::_instance {nullptr};


GuiApplication::GuiApplication(int& argc, char *argv[]) :
  QGuiApplication(argc, argv), _window( std::make_shared<Window>() ), _gmlib(nullptr)
{

  assert(!_instance);
  _instance = std::unique_ptr<GuiApplication>(this);

  connect( _window.get(), &Window::sceneGraphInitialized, this, &GuiApplication::onSGInit );

  _window->show();
}

void GuiApplication::onSGInit() {

  QOpenGLContext *ctx = _window->openglContext();
  if( !ctx->isValid() ) {

    qCritical() << "OpenGLContext not valid";
    exit(1);
  }
  qDebug() << "Initialized GLformat: " << ctx->format();

  // Init GMlibWrapper
  _gmlib = std::make_shared<GMlibWrapper>(ctx);
  connect( _gmlib.get(),  &GMlibWrapper::signFrameReady,   _window.get(), &Window::signFrameReady );
  connect( _window.get(), &Window::signGuiViewportChanged, _gmlib.get(),  &GMlibWrapper::changeRenderGeometry );

  // Load gui qml
  _window->setSource( QUrl("qrc:/qml/main.qml") );

  // Init test scene of the GMlib wrapper
  _gmlib->initTestScene();

  // Start simulator
  _gmlib->start();

}

const GuiApplication&
GuiApplication::getInstance() {

  return *_instance;
}
