// gmlib
namespace GMlib {

  class Scene;
  class Camera;
  class DefaultRenderer;
  class SelectRenderer;

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
  void                      changeRenderGeometry( const QString& name, const QRectF &new_geometry );


  void                      moveObjFw();
  void                      moveObjBw();
  void                      moveObjLeft();
  void                      moveObjRight();

  void                      select( int x, int y );

protected:
  void                      timerEvent(QTimerEvent *e);

private:
  int                       _timer_id;
  QOpenGLContext*           _context;
  QOffscreenSurface*        _offscreensurface;
  GMlib::Scene*             _scene;

  GMlib::DefaultRenderer*   _proj_renderer;
  GMlib::DefaultRenderer*   _front_renderer;
  GMlib::DefaultRenderer*   _side_renderer;
  GMlib::DefaultRenderer*   _top_renderer;

  GMlib::SelectRenderer*    _select_renderer;


  GMlib::PSurf<float,3>*    _world;
  GMlib::PSurf<float,3>*    _obj;

  GMlib::Point<float,2>     _obj_pos;

  GMlib::Camera*            _proj_cam;
  GMlib::Camera*            _front_cam;
  GMlib::Camera*            _side_cam;
  GMlib::Camera*            _top_cam;


  void                      moveObj( const GMlib::Vector<float,2>& dir );

signals:
  void                      signFrameReady();
};
