#include "gmlibwrapper.h"

// GMlib
#include <gmOpenGLModule>
#include <gmSceneModule>

// Qt
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QRectF>

GMlibWrapper::GMlibWrapper(QOpenGLContext *top_context, const QSize &initial_render_size) :
  QObject(), _timer_id(0), _tex_size(initial_render_size), _fbo(0) //, _frames(0), _frames_avg_duration(2000), _time_accu(0)
{

  // Create Internal shared GL context
  _context = new QOpenGLContext;
  _context->setShareContext( top_context );
  _context->setFormat( top_context->format() );
  _context->create();

  // Set up offscreen rendering surface for GMlib rendering
  _offscreensurface = new QOffscreenSurface;
  _offscreensurface->setFormat(_context->format());
  _offscreensurface->create();

  _context->makeCurrent( _offscreensurface ); {

    // Setup and initialized GMlib GL backend
    GMlib::GL::OGL::init();

    // Setup and init the GMlib GMWindow
    _gmwindow = new GMlib::GMWindow;
    _gmwindow->init();

    // Setup a texture render target and resize the window
    _gmwindow->setRenderTarget( new GMlib::RenderTexture("display_render_target") );
    _gmwindow->reshape( _tex_size.width(), _tex_size.height() );

  } _context->doneCurrent();
}

GMlibWrapper::~GMlibWrapper() {}

void GMlibWrapper::changeRenderGeometry(const QRectF& new_geometry) {

  _tex_size = new_geometry.size().toSize();

  if( _tex_size.width() <= 0 || _tex_size.height() <= 0 )
    return;

  stop();


  _context->makeCurrent(_offscreensurface); {

    _gmwindow->reshape( _tex_size.width(), _tex_size.height() );

  } _context->doneCurrent();

  start();
}

void GMlibWrapper::timerEvent(QTimerEvent* e) {

  if( !_context->isValid() )
    return;

  _context->makeCurrent(_offscreensurface); {

    _gmwindow->prepare();
    _gmwindow->simulate();
    _gmwindow->render();

    emit signFrameReady();

  } _context->doneCurrent();
}

void GMlibWrapper::start() {

  _timer_id = startTimer(16, Qt::PreciseTimer);
  _gmwindow->toggleRun();
}

void GMlibWrapper::stop() {

  _gmwindow->toggleRun();
  killTimer(_timer_id);
}
