#ifndef GUIAPPLICATION_H
#define GUIAPPLICATION_H


//#include <standardhidmanager.h>

// qt
#include <QGuiApplication>


class Window;
class GMlibWrapper;


class GuiApplication : public QGuiApplication {
  Q_OBJECT
public:
  explicit GuiApplication(int& argc, char* argv[]);
  ~GuiApplication();


//  StandardHidManager*         getHidManager() const;

private:
  Window*                     _window;
  GMlibWrapper*               _gmlib;
//  StandardHidManager*         _hidmanager;

private slots:
  void                        onSGInit();



  void                        handleMousePressEvent( const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y );
  void                        handleMouseReleaseEvent( const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y );
  void                        handleMousePositionChangedEvent( const QString& view_name, int buttons, int modifiers, bool was_held, int x, int y );

  void                        handleKeyPressEvent( const QString& view_name, int key, int modifiers );
  void                        handleKeyReleaseEvent( const QString& view_name, int key, int modifiers );

private:
  static GuiApplication*      _instance;
public:
  static GuiApplication*      getInstance();
};

#endif // GUIAPPLICATION_H
