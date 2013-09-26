#include "gmlibwrapper.h"

// GMlib
#include <gmOpenGLModule>
#include <gmSceneModule>
#include <gmParametricsModule>

// Qt
#include <QTimerEvent>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QRectF>
#include <QDebug>

GMlibWrapper::GMlibWrapper(QOpenGLContext *top_context, const QSize &initial_render_size) :
  QObject(), _timer_id(0), _tex_size(initial_render_size)
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
    _gmwindow = new GMlib::GMWindow(false);
    _gmwindow->init();

    // Setup a texture render target and resize the window
    _gmwindow->setRenderTarget( new GMlib::RenderTexture("display_render_target") );

    setupTestScene();

    _gmwindow->reshape( _tex_size.width(), _tex_size.height() );

  } _context->doneCurrent();
}

GMlibWrapper::~GMlibWrapper() {

  stop();
}

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

void GMlibWrapper::moveObjFw() {

  qDebug() << "Move obj Fw";
  moveObj( GMlib::Vector<float,2>( 0.05, 0.0 ) );
}

void GMlibWrapper::moveObjBw() {

  qDebug() << "Move obj Bw";
  moveObj( GMlib::Vector<float,2>( -0.05, 0.0 ) );
}

void GMlibWrapper::moveObjLeft() {

  qDebug() << "Move obj Left";
  moveObj( GMlib::Vector<float,2>( 0.0, -0.05 ) );
}

void GMlibWrapper::moveObjRight() {

  qDebug() << "Move obj Right";
  moveObj( GMlib::Vector<float,2>( 0.0, 0.05 ) );
}

void GMlibWrapper::timerEvent(QTimerEvent* e) {

  qDebug() << "Timer Event!!";


  if( !_context->isValid() )
    return;

  qDebug() << "  Valid context";

  e->accept();

  _context->makeCurrent(_offscreensurface); {

    _gmwindow->prepare();
    _gmwindow->simulate();
    _gmwindow->render();

  } _context->doneCurrent();


  emit signFrameReady();
}

void GMlibWrapper::moveObj(const GMlib::Vector<float,2>& dir) {

  _obj_pos += dir;

  if( _obj_pos(0) > 1.0 ) _obj_pos[0] -= 1.0;
  if( _obj_pos(1) > 1.0 ) _obj_pos[1] -= 1.0;

  double u, v;
  u = _world->getParStartU() + _world->getParDeltaU() * _obj_pos(0);
  u = _world->getParStartV() + _world->getParDeltaV() * _obj_pos(1);

  _context->makeCurrent(_offscreensurface ); {

    GMlib::DMatrix< GMlib::Vector<float,3> > w_eval = _world->evaluateGlobal( u, v, 1, 1 );
    _obj->translateGlobal( (w_eval(0)(0) - _obj->getPos()) );

    GMlib::Vector<float,3> n_corr = w_eval(1)(0) ^ w_eval(0)(1);
    n_corr.normalize();
    n_corr *= _obj->getSurroundingSphere().getRadius();
    _obj->translateGlobal(n_corr);

  } _context->doneCurrent();
}

void GMlibWrapper::start() {

  if( _timer_id )
    return;

  _timer_id = startTimer(16, Qt::PreciseTimer);
  _gmwindow->toggleRun();
}

void GMlibWrapper::stop() {

  if( !_timer_id )
    return;

  _gmwindow->toggleRun();
  killTimer(_timer_id);
  _timer_id = 0;
}

void GMlibWrapper::setupTestScene() {


  GMlib::Point<float,3> init_cam_pos(  0.0f, 0.0f, 0.0f );
  GMlib::Vector<float,3> init_cam_dir( 0.0f, 1.0f, 0.0f );
  GMlib::Vector<float,3> init_cam_up(  0.0f, 0.0f, 1.0f );

  GMlib::Camera *cam_project = new GMlib::Camera( init_cam_pos, init_cam_dir, init_cam_up );
  cam_project->setCuttingPlanes( 1.0f, 8000.0f );
  cam_project->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
  cam_project->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );


  // Front
  GMlib::Camera *cam_front = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
  cam_front->setCuttingPlanes( 1.0f, 8000.0f );
  //  cam_front->zoom( 5.0 );

  // Side
  GMlib::Camera *cam_side = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
  cam_side->setCuttingPlanes( 1.0f, 8000.0f );
  //  cam_side->zoom( 5.0 );

  // Up
  GMlib::Camera *cam_top = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
  cam_top->setCuttingPlanes( 1.0f, 8000.0f );
  //  cam_top->zoom( 10.0 );


  _gmwindow->insertCamera( cam_project );
  _gmwindow->insertCamera( cam_front );
  _gmwindow->insertCamera( cam_side );
  _gmwindow->insertCamera( cam_top );


  int cam_project_idx = _gmwindow->getCameraIndex( cam_project );
  int cam_front_idx   = _gmwindow->getCameraIndex( cam_front );
  int cam_side_idx    = _gmwindow->getCameraIndex( cam_side );
  int cam_top_idx     = _gmwindow->getCameraIndex( cam_top );


  _gmwindow->addViewSet( cam_project_idx );
  _gmwindow->addToViewSet( cam_top_idx, cam_project_idx, false );
  _gmwindow->addToViewSet( cam_front_idx, cam_top_idx, true );
  _gmwindow->addToViewSet( cam_side_idx, cam_front_idx, false );


  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );

  _world = new GMlib::PTorus<float>();
  _world->toggleDefaultVisualizer();
  _world->replot( 200, 200, 1, 1 );
  _gmwindow->insert(_world);

  _obj = new GMlib::PSphere<float>(2);
  _obj->toggleDefaultVisualizer();
  _obj->replot( 200, 200, 1, 1 );
  _gmwindow->insert(_obj);

  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );




}
