// gmlib
namespace GMlib {

  class Scene;
  class Camera;
  class DefaultRendererWithSelect;

  template<typename T, int n>
  class PSurf;
}

#include "core/gmpoint"

// qt
#include <QObject>
#include <QSize>
class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFramebufferObject;


// stl
#include <memory>
#include <unordered_map>


struct RenderCamPair {
  RenderCamPair() : render{nullptr}, camera{nullptr} {}
  GMlib::DefaultRendererWithSelect*   render;
  GMlib::Camera*                      camera;
};




class GMlibWrapper : public QObject {
  Q_OBJECT
public:
  explicit GMlibWrapper( QOpenGLContext *top_ctx, const QSize& initial_render_size );
  ~GMlibWrapper();

  void                      start();
  void                      stop();

  void                      initScene();


  GMlib::Scene*             getScene() const;

public slots:
  void                      changeRenderGeometry( const QString& name, const QRectF &new_geometry );

  void                      select( int x, int y );

protected:
  void                      timerEvent(QTimerEvent *e);

private:
  int                       _timer_id;
  QOpenGLContext*           _context;
  QOffscreenSurface*        _offscreensurface;
  GMlib::Scene*             _scene;

  GMlib::DefaultRendererWithSelect*   _proj_renderer;
  GMlib::DefaultRendererWithSelect*   _front_renderer;
  GMlib::DefaultRendererWithSelect*   _side_renderer;
  GMlib::DefaultRendererWithSelect*   _top_renderer;

  std::unordered_map<std::string,RenderCamPair>   _rc_pairs;




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
