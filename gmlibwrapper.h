// gmlib
namespace GMlib {

  class Scene;
  class Camera;
  class DefaultRenderer;
  class TextureRenderTarget;

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
  explicit GMlibWrapper( QOpenGLContext *top_ctx);
  ~GMlibWrapper();

  void                                  start();
  void                                  stop();

  const std::shared_ptr<GMlib::Scene>&  getScene() const;
  const GMlib::TextureRenderTarget&     getRenderTextureOf( const std::string& name ) const;


  void                                  initTestScene();

public slots:
  void                                  changeRenderGeometry( const QString& name,
                                                              const QRectF &new_geometry );

protected:
  void                                  timerEvent(QTimerEvent *e);



private:
  int                                               _timer_id;
  std::shared_ptr<QOpenGLContext>                   _context;
  std::shared_ptr<QOffscreenSurface>                _offscreensurface;
  std::shared_ptr<GMlib::Scene>                     _scene;
  std::unordered_map<std::string, RenderCamPair>    _rc_pairs;

  std::shared_ptr<GMlib::PSurf<float,3>>            _world;
  std::shared_ptr<GMlib::PSurf<float,3>>            _obj;
  GMlib::Point<float,2>                             _obj_pos;

  void                                              moveObj( const GMlib::Vector<float,2>& dir );

signals:
  void                                              signFrameReady();



  // "Singleton instance"
private:
  static std::unique_ptr<GMlibWrapper>      _instance;
public:
  static const GMlibWrapper&                getInstance();
};
