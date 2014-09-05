#include "gmlibwrapper.h"


#include "testtorus.h"


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

GMlibWrapper::GMlibWrapper(QOpenGLContext *top_context, const QSize &render_size)
  : QObject(), _timer_id(0), /*_tex_size(initial_render_size),*/
    _proj_cam(0x0), _front_cam(0x0), _top_cam(0x0), _side_cam(0x0)
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
    GMlib::GL::OpenGLManager::init();

    // Setup and init the GMlib GMWindow
    _scene = new GMlib::Scene;

    _proj_renderer = new GMlib::DefaultRenderer;
    _proj_renderer->setRenderTarget(new GMlib::RenderTexture("projection_camera"));

    _front_renderer = new GMlib::DefaultRenderer;
    _front_renderer->setRenderTarget(new GMlib::RenderTexture("front_camera"));

    _side_renderer = new GMlib::DefaultRenderer;
    _side_renderer->setRenderTarget(new GMlib::RenderTexture("side_camera"));

    _top_renderer = new GMlib::DefaultRenderer;
    _top_renderer->setRenderTarget(new GMlib::RenderTexture("top_camera"));

    _select_renderer = new GMlib::SelectRenderer;

    // Setup a texture render target and resize the window
//    _scene->setRenderTarget( new GMlib::RenderTexture("display_render_target") );

    setupTestScene();

//    _gmwindow->reshape( _tex_size.width(), _tex_size.height() );

  } _context->doneCurrent();
}

GMlibWrapper::~GMlibWrapper() {

  stop();
}

void GMlibWrapper::changeRenderGeometry(const QString& name, const QRectF& new_geometry) {

  QSize tex_size = new_geometry.size().toSize();

  if( tex_size.width() <= 0 || tex_size.height() <= 0 )
    return;

  stop();

  _context->makeCurrent(_offscreensurface); {

    if( name == "projection_camera" && _proj_cam ) {
      _proj_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
      _select_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
    }
    else if( name == "front_camera" && _front_cam )
      _front_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
    else if( name == "side_camera" && _side_cam )
      _side_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
    else if( name == "top_camera" && _top_cam )
      _top_renderer->setViewport(0,0,tex_size.width(),tex_size.height());

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

  if( !_context->isValid() )
    return;

  e->accept();

  _context->makeCurrent(_offscreensurface); {

    _scene->prepare();
    _scene->simulate();
    _proj_renderer->render();
    _front_renderer->render();
    _side_renderer->render();
    _top_renderer->render();

  } _context->doneCurrent();


  emit signFrameReady();
}

void GMlibWrapper::moveObj(const GMlib::Vector<float,2>& dir) {

  _obj_pos += dir;

  if( _obj_pos(0) > 1.0 ) _obj_pos[0] -= 1.0;
  if( _obj_pos(1) > 1.0 ) _obj_pos[1] -= 1.0;

  double u, v;
  u = _world->getParStartU() + _world->getParDeltaU() * _obj_pos(0);
  v = _world->getParStartV() + _world->getParDeltaV() * _obj_pos(1);

  _context->makeCurrent(_offscreensurface ); {

    GMlib::DMatrix< GMlib::Vector<float,3> > w_eval = _world->evaluateGlobal( u, v, 1, 1 );
//    _obj->translateGlobal( (w_eval(0)(0) - _obj->getPos()) );

    GMlib::Vector<float,3> n_corr = w_eval(1)(0) ^ w_eval(0)(1);
    n_corr.normalize();
    n_corr *= _obj->getSurroundingSphere().getRadius();
    _obj->translateGlobal(n_corr + (w_eval(0)(0) - _obj->getPos()) );

  } _context->doneCurrent();
}

void GMlibWrapper::start() {

  if( _timer_id )
    return;

  _timer_id = startTimer(16, Qt::PreciseTimer);
  _scene->toggleRun();
}

void GMlibWrapper::stop() {

  if( !_timer_id )
    return;

  _scene->toggleRun();
  killTimer(_timer_id);
  _timer_id = 0;
}

void GMlibWrapper::setupTestScene() {

  // Insert a light
  GMlib::Point<GLfloat,3> init_light_pos( 2.0, 4.0, 10 );
  GMlib::PointLight *pl = new GMlib::PointLight(
                            GMlib::GMcolor::White, GMlib::GMcolor::White,
                            GMlib::GMcolor::White, init_light_pos );
  pl->setAttenuation(0.8, 0.002, 0.0008);
  _scene->insertLight( pl, false );

  // Insert Sun
  _scene->insertSun();

  GMlib::Point<float,3> init_cam_pos(  0.0f, 0.0f, 0.0f );
  GMlib::Vector<float,3> init_cam_dir( 0.0f, 1.0f, 0.0f );
  GMlib::Vector<float,3> init_cam_up(  0.0f, 0.0f, 1.0f );

  _proj_cam = new GMlib::Camera( init_cam_pos, init_cam_dir, init_cam_up );
  _proj_cam->setCuttingPlanes( 1.0f, 8000.0f );
  _proj_cam->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
  _proj_cam->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );


  // Front
  _front_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
  _front_cam->setCuttingPlanes( 1.0f, 8000.0f );
  //  _front_cam->zoom( 5.0 );

  // Side
  _side_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
  _side_cam->setCuttingPlanes( 1.0f, 8000.0f );
  //  _side_cam->zoom( 5.0 );

  // Up
  _top_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
  _top_cam->setCuttingPlanes( 1.0f, 8000.0f );
  //  _top_cam->zoom( 10.0 );


//  _proj_cam->getRenderer()->setRenderTarget( new GMlib::RenderTexture("projection_camera") );
//  _front_cam->getRenderer()->setRenderTarget( new GMlib::RenderTexture("front_camera") );
//  _side_cam->getRenderer()->setRenderTarget( new GMlib::RenderTexture("side_camera") );
//  _top_cam->getRenderer()->setRenderTarget( new GMlib::RenderTexture("top_camera") );


  _scene->insertCamera( _proj_cam );
  _scene->insertCamera( _front_cam );
  _scene->insertCamera( _side_cam );
  _scene->insertCamera( _top_cam );


  _proj_renderer->setCamera(_proj_cam);
  _front_renderer->setCamera(_front_cam);
  _side_renderer->setCamera(_side_cam);
  _top_renderer->setCamera(_top_cam);


  _select_renderer->setCamera(_proj_cam);


//  int cam_project_idx = _gmwindow->getCameraIndex( cam_project );
//  int cam_front_idx   = _gmwindow->getCameraIndex( cam_front );
//  int cam_side_idx    = _gmwindow->getCameraIndex( cam_side );
//  int cam_top_idx     = _gmwindow->getCameraIndex( cam_top );


//  _gmwindow->addViewSet( cam_project_idx );
//  _gmwindow->addToViewSet( cam_top_idx, cam_project_idx, false );
//  _gmwindow->addToViewSet( cam_front_idx, cam_top_idx, true );
//  _gmwindow->addToViewSet( cam_side_idx, cam_front_idx, false );


  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );

//  _world = new GMlib::PTorus<float>();
  _world = new TestTorus;
  _world->toggleDefaultVisualizer();
  _world->replot( 200, 200, 1, 1 );
  _scene->insert(_world);

  _obj = new GMlib::PSphere<float>(2);
  _obj->toggleDefaultVisualizer();
  _obj->replot( 200, 200, 1, 1 );
  _world->insert(_obj);

  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );

  moveObj( GMlib::Vector<float,2>(0.0f,0.0f) );

}



void GMlibWrapper::select(int x, int y) {

  qDebug() << "Mouse clicked at " << x << " " << y;


  _context->makeCurrent(_offscreensurface ); {

    y = _select_renderer->getViewportH() - y - 1;

    qDebug() << "Select: " << x << " " << y;

    _select_renderer->select(-GMlib::GM_SO_TYPE_SELECTOR);

    GMlib::DisplayObject* so = _select_renderer->findObject(x,y);
    if( so ) {
      qDebug() << "DO found: " << so->getIdentity().c_str();
      so->toggleSelected();
    }
    else {
      qDebug() << "No DO found";
    }




//    GMlib::Camera *ac = _gmwindow->findCamera( GMlib::Vector<int,2>(x,y) );
//    if(!ac) {
//      qDebug() << "Active camera not found in GMWindow!!";
//      return;
//    }
//    GMlib::SceneObject *so = ac->findSelectObject( x, y, -GMlib::GM_SO_TYPE_SELECTOR );
//  //  GMlib::SceneObject *so = _gmwindow->findSelectObject( ac, x, y, -GMlib::GM_SO_TYPE_SELECTOR );
//    if( so ) {
//      qDebug() << "SO found: " << so->getIdentity().c_str();
//      so->toggleSelected();
//    }
//    else {
//      qDebug() << "No SO found";
//    }
  }
}
