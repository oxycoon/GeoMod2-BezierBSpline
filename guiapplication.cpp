#include "guiapplication.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QDebug>

// stl
#include <cassert>


std::unique_ptr<GuiApplication> GuiApplication::_instance {nullptr};


GuiApplication::GuiApplication(int& argc, char *argv[]) :
  QGuiApplication(argc, argv), _window{std::make_shared<Window>()}, _gmlib{nullptr}
{

  assert(!_instance);
  _instance = std::unique_ptr<GuiApplication>(this);

  connect( _window.get(), &Window::sceneGraphInitialized, this, &GuiApplication::onSGInit );

  _window->show();
}

void GuiApplication::onSGInit() {

  // Init GMlibWrapper
  _gmlib = std::make_shared<GMlibWrapper>(_window->openglContext());
  connect( _gmlib.get(),  &GMlibWrapper::signFrameReady,   _window.get(), &Window::signFrameReady );
  connect( _window.get(), &Window::signGuiViewportChanged, _gmlib.get(),  &GMlibWrapper::changeRenderGeometry );

  // Load gui qml
  _window->setSource( QUrl("qrc:/qml/main.qml") );

  connect( _window.get(), &Window::signMousePressed, this, &GuiApplication::mousePressed );

  // Init test scene of the GMlib wrapper
  _gmlib->initScene();

  // Start simulator
  _gmlib->start();

}

void GuiApplication::mousePressed(const QString& render_name, const QPointF& pos) {

  qDebug() << "Mouse pressed: " << pos << " in render <" << render_name << ">";
}

const GuiApplication&
GuiApplication::getInstance() {

  return *_instance;
}
