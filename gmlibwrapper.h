#include "glcontextsurfacewrapper.h"

// gmlib
#include "core/gmpoint"

namespace GMlib {

  class Scene;
  class Camera;
  class DefaultRenderer;
  class DefaultSelectRenderer;
  class TextureRenderTarget;

  template<typename T, int n>
  class PSurf;
}

// qt
#include <QObject>
#include <QSize>
#include <QRectF>
class QOpenGLContext;
class QOpenGLFramebufferObject;


// stl
#include <memory>
#include <unordered_map>


struct RenderCamPair {
  RenderCamPair() : render{nullptr}, camera{nullptr} {}
  GMlib::DefaultRenderer*       render;
  GMlib::Camera*                camera;
  struct {
    QRectF                      geometry { QRectF(0,0,200,200) };
    bool                        changed {true};
  } viewport;
};





class GMlibWrapper : public QObject {
  Q_OBJECT
//private:
//  explicit GMlibWrapper();
public:
  explicit GMlibWrapper( QOpenGLContext* context );
  ~GMlibWrapper();

  void                                  start();
  void                                  stop();

  const std::shared_ptr<GMlib::Scene>&  getScene() const;
  const GMlib::TextureRenderTarget&     getRenderTextureOf( const std::string& name ) const;

  void                                  initScene();


public slots:
  void                                  changeRenderGeometry( const QString& name,
                                                              const QRectF &new_geometry );


  void                                  mousePressed( const QString& name, const QPointF& pos );

protected:
  void                                  timerEvent(QTimerEvent *e);


private:
  int                                               _timer_id;
  GLContextSurfaceWrapper                           _glsurface;

  std::shared_ptr<GMlib::Scene>                     _scene;
  std::unordered_map<std::string, RenderCamPair>    _rc_pairs;
  std::shared_ptr<GMlib::DefaultSelectRenderer>     _select_renderer;

  std::shared_ptr<GMlib::PSurf<float,3>>            _torus;

signals:
  void                                              signFrameReady();



  // "Singleton instance"
private:
  static std::unique_ptr<GMlibWrapper>      _instance;
public:
  static const GMlibWrapper&                getInstance();
};
