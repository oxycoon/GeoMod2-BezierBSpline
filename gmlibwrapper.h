// gmlib
namespace GMlib { class GMWindow; }

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

public slots:
  void                      changeRenderGeometry( const QRectF &new_geometry );

protected:
  void                      timerEvent(QTimerEvent *e);

private:
  int                       _timer_id;
  QOpenGLContext*           _context;
  QOffscreenSurface*        _offscreensurface;
  QSize                     _tex_size;
  GMlib::GMWindow*          _gmwindow;
  QOpenGLFramebufferObject* _fbo;

signals:
  void                      signFrameReady();
};
