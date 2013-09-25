// gmlib
namespace GMlib {

  class GMWindow;

  template<typename T, int n>
  class PSurf;
}
#include "core/gmPoint"

// qt
#include <QObject>
#include <QSize>
class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFramebufferObject;

class GMlibWrapper : public QObject {
  Q_OBJECT
public:
  explicit GMlibWrapper( QOpenGLContext *top_ctx, const QSize& initial_render_size );
  ~GMlibWrapper();

  void                      start();
  void                      stop();

  void                      setupTestScene();

public slots:
  void                      changeRenderGeometry( const QRectF &new_geometry );


  void                      moveObjFw();
  void                      moveObjBw();
  void                      moveObjLeft();
  void                      moveObjRight();

protected:
  void                      timerEvent(QTimerEvent *e);

private:
  int                       _timer_id;
  QOpenGLContext*           _context;
  QOffscreenSurface*        _offscreensurface;
  QSize                     _tex_size;
  GMlib::GMWindow*          _gmwindow;


  GMlib::PSurf<double,3>*   _world;
  GMlib::PSurf<double,3>*   _obj;

  GMlib::Point<double,2>    _obj_pos;


  void                      moveObj( const GMlib::Vector<double,2>& dir );

signals:
  void                      signFrameReady();
};
