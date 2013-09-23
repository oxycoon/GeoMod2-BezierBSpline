#ifndef GUIAPPLICATION_H
#define GUIAPPLICATION_H


// qt
#include <QGuiApplication>


class Window;
class GMlibWrapper;


class GuiApplication : public QGuiApplication {
  Q_OBJECT
public:
  explicit GuiApplication(int argc, char* argv[]);

private:
  Window*         _window;
  GMlibWrapper*   _gmlib;

private slots:
  void            onSGInit();
};

#endif // GUIAPPLICATION_H
