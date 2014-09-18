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
#include <QRectF>
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
  struct {
    QRectF                            geometry { QRectF(0,0,200,200) };
    bool                              changed {true};
  } viewport;
};





class GMlibWrapper : public QObject {
  Q_OBJECT
public:
  explicit GMlibWrapper( QOpenGLContext *top_ctx);
  ~GMlibWrapper();

  void                      start();
  void                      stop();

  void                      initScene();


  GMlib::Scene*             getScene() const;

public slots:
  void                      changeRenderGeometry( const QString& name, const QRectF &new_geometry );

//  void                      select( int x, int y );

protected:
  void                      timerEvent(QTimerEvent *e);

private:
  int                       _timer_id;
  QOpenGLContext*           _context;
  QOffscreenSurface*        _offscreensurface;
  GMlib::Scene*             _scene;

  std::unordered_map<std::string,RenderCamPair>   _rc_pairs;

  GMlib::PSurf<float,3>*    _world;
  GMlib::PSurf<float,3>*    _obj;

  GMlib::Point<float,2>     _obj_pos;


  void                      moveObj( const GMlib::Vector<float,2>& dir );

signals:
  void                      signFrameReady();
};
