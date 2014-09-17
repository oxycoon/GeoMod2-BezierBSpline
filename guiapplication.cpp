#include "guiapplication.h"

// local
#include "window.h"
#include "gmlibwrapper.h"

// qt
#include <QOpenGLContext>
#include <QQuickItem>
#include <QDebug>


GuiApplication* GuiApplication::_instance = nullptr;


GuiApplication::GuiApplication(int& argc, char *argv[]) :
  QGuiApplication(argc, argv)
{

  if(!_instance) _instance = this;

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
  connect( _window, &Window::signGuiViewportChanged,
           _gmlib,  &GMlibWrapper::changeRenderGeometry );

  _gmlib->start();

  // Load gui qml
  _window->setSource( QUrl("qrc:/qml/main.qml") );

  // Set up hidmanager
//  _hidmanager = new StandardHidManager(_gmlib->getScene(),this);

  connect( _window->rootObject(), SIGNAL(mousePressed(QString,int,int,bool,int,int)),
           this, SLOT(handleMousePressEvent(QString,int,int,bool,int,int)) );
  connect( _window->rootObject(), SIGNAL(mouseReleased(QString,int,int,bool,int,int)),
           this, SLOT(handleMouseReleaseEvent(QString,int,int,bool,int,int)) );
  connect( _window->rootObject(), SIGNAL(mousePositionChanged(QString,int,int,bool,int,int)),
           this, SLOT(handleMousePositionChangedEvent(QString,int,int,bool,int,int)) );

  connect( _window->rootObject(), SIGNAL(keyPressed(QString,int,int)),
           this, SLOT(handleKeyPressEvent(QString,int,int)) );
  connect( _window->rootObject(), SIGNAL(keyReleased(QString,int,int)),
           this, SLOT(handleKeyReleaseEvent(QString,int,int)) );
}

GuiApplication::~GuiApplication() {

//  delete _hidmanager;
  delete _gmlib;
  delete _window;
}

//StandardHidManager*GuiApplication::getHidManager() const {

////  return _hidmanager;
//}

void GuiApplication::handleMousePressEvent(const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y) {

//  qDebug() << "Mouse pressed:";
//  qDebug() << "  view name: " << view_name;
//  qDebug() << "  buttons: " << buttons;
//  qDebug() << "  modifiers: " << modifiers;
//  qDebug() << "  was held: " << was_held;
//  qDebug() << "  x: " << x;
//  qDebug() << "  y: " << y;

  if( view_name == "Projection" ) {
    qDebug() << "Selecting projection";

    _gmlib->select(x,y);
  }
}

void GuiApplication::handleMouseReleaseEvent(const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y) {

//  qDebug() << "Mouse released:";
//  qDebug() << "  view name: " << view_name;
//  qDebug() << "  buttons: " << buttons;
//  qDebug() << "  modifiers: " << modifiers;
//  qDebug() << "  was held: " << was_held;
//  qDebug() << "  x: " << x;
//  qDebug() << "  y: " << y;
}

void GuiApplication::handleMousePositionChangedEvent(const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y) {

//  qDebug() << "Mouse position changed:";
//  qDebug() << "  view name: " << view_name;
//  qDebug() << "  buttons: " << buttons;
//  qDebug() << "  modifiers: " << modifiers;
//  qDebug() << "  was held: " << was_held;
//  qDebug() << "  x: " << x;
//  qDebug() << "  y: " << y;
}

void GuiApplication::handleKeyPressEvent(const QString& view_name, int key, int modifiers) {

//  qDebug() << "Key press event:";
//  qDebug() << "  view name: " << view_name;
//  qDebug() << "  key: " << key;
//  qDebug() << "  modifiers: " << modifiers;
}

void GuiApplication::handleKeyReleaseEvent(const QString& view_name, int key, int modifiers) {

//  qDebug() << "Key release event:";
//  qDebug() << "  view name: " << view_name;
//  qDebug() << "  key: " << key;
//  qDebug() << "  modifiers: " << modifiers;
}

GuiApplication*
GuiApplication::getInstance() {

  return _instance;
}
