#include "guiapplication.h"

#include "glcontextsurfacewrapper.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QDebug>

// stl
#include <cassert>


std::unique_ptr<GuiApplication> GuiApplication::_instance {nullptr};


GuiApplication::GuiApplication(int& argc, char *argv[])
  : QGuiApplication(argc, argv),
    _window{std::make_shared<Window>()}, _gmlib{nullptr}, _glsurface{nullptr}
{

  assert(!_instance);
  _instance = std::unique_ptr<GuiApplication>(this);

  connect( _window.get(), &Window::sceneGraphInitialized, this, &GuiApplication::onSGInit );

  _window->show();
}

GuiApplication::~GuiApplication() {

  _glsurface->makeCurrent(); {

    _gmlib.reset();
    _window.reset();

  } _glsurface->doneCurrent();
}

void
GuiApplication::onSGInit() {

  // Init GLSurface
  _window->initGLSurface();

  // Init GMlibWrapper
  _glsurface = _window->getGLSurface();
  _gmlib = std::make_shared<GMlibWrapper>(_glsurface);
  connect( _gmlib.get(),  &GMlibWrapper::signFrameReady,   _window.get(), &Window::signFrameReady );
  connect( _window.get(), &Window::signGuiViewportChanged, _gmlib.get(),  &GMlibWrapper::changeRenderGeometry );

  // Load gui qml
  _window->setSource( QUrl("qrc:/qml/main.qml") );

  connect( _window.get(), &Window::signMousePressed, _gmlib.get(), &GMlibWrapper::mousePressed );
  connect( _window.get(), &Window::signKeyPressed, _gmlib.get(), &GMlibWrapper::keyPressed );

  // Init test scene of the GMlib wrapper
  _gmlib->initScene();

  // Start simulator
  _gmlib->start();
}

const GuiApplication&
GuiApplication::getInstance() {

  return *_instance;
}
