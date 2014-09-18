#include "gmlibwrapper.h"


#include "testtorus.h"


// GMlib
#include <gmOpenglModule>
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

//    _proj_renderer = new GMlib::DefaultRendererWithSelect;
//    _proj_renderer->setRenderTarget(new GMlib::RenderTexture("Projection"));
////    _proj_renderer->setSelectRboName("select_proj");

//    _front_renderer = new GMlib::DefaultRendererWithSelect;
//    _front_renderer->setRenderTarget(new GMlib::RenderTexture("Front"));

//    _side_renderer = new GMlib::DefaultRendererWithSelect;
//    _side_renderer->setRenderTarget(new GMlib::RenderTexture("Side"));

//    _top_renderer = new GMlib::DefaultRendererWithSelect;
//    _top_renderer->setRenderTarget(new GMlib::RenderTexture("Top"));


  } _context->doneCurrent();

  initScene();
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


    if( _rc_pairs.count(name.toStdString()) > 0 && _rc_pairs[name.toStdString()].camera != nullptr )
      _rc_pairs[name.toStdString()].render->setViewport(0,0,tex_size.width(),tex_size.height());

//    if( name == "Projection" && _proj_cam ) {
//      _proj_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
//    }
//    else if( name == "Front" && _front_cam )
//      _front_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
//    else if( name == "Side" && _side_cam )
//      _side_renderer->setViewport(0,0,tex_size.width(),tex_size.height());
//    else if( name == "Top" && _top_cam )
//      _top_renderer->setViewport(0,0,tex_size.width(),tex_size.height());

  } _context->doneCurrent();

  start();
}

void GMlibWrapper::timerEvent(QTimerEvent* e) {

  if( !_context->isValid() )
    return;


  e->accept();

  _context->makeCurrent(_offscreensurface); {

    _scene->prepare();
    _scene->simulate();
    for( auto& rc_pair : _rc_pairs )
      rc_pair.second.render->render();

//    _proj_renderer->render();
//    _front_renderer->render();
//    _side_renderer->render();
//    _top_renderer->render();

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

void GMlibWrapper::initScene() {

  _context->makeCurrent(_offscreensurface ); {

    // Insert a light
    GMlib::Point<GLfloat,3> init_light_pos( 2.0, 4.0, 10 );
    GMlib::PointLight *pl = new GMlib::PointLight(
                              GMlib::GMcolor::White, GMlib::GMcolor::White,
                              GMlib::GMcolor::White, init_light_pos );
    pl->setAttenuation(0.8, 0.002, 0.0008);
    _scene->insertLight( pl, false );

    // Insert Sun
    _scene->insertSun();




    int init_viewport_size = 600;
    GMlib::Point<float,3> init_cam_pos(  0.0f, 0.0f, 0.0f );
    GMlib::Vector<float,3> init_cam_dir( 0.0f, 1.0f, 0.0f );
    GMlib::Vector<float,3> init_cam_up(  0.0f, 0.0f, 1.0f );

    _rc_pairs.reserve(4);
    _rc_pairs["Projection"] = RenderCamPair {};
    _rc_pairs["Front"]      = RenderCamPair {};
    _rc_pairs["Side"]       = RenderCamPair {};
    _rc_pairs["Top"]        = RenderCamPair {};

    for( auto& rcpair : _rc_pairs ) {

      rcpair.second.render = new GMlib::DefaultRendererWithSelect;
      rcpair.second.camera = new GMlib::Camera;
      rcpair.second.render->setCamera(rcpair.second.camera);
    }

    // Projection cam
    auto& proj_rcpair = _rc_pairs["Projection"];
    proj_rcpair.render->setRenderTarget(new GMlib::RenderTexture("Projection"));
    proj_rcpair.camera->set(init_cam_pos,init_cam_dir,init_cam_up);
    proj_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    proj_rcpair.camera->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
    proj_rcpair.camera->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );
    _scene->insertCamera( proj_rcpair.camera );
    proj_rcpair.render->setViewport( 0, 0, init_viewport_size, init_viewport_size );

    // Front cam
    auto& front_rcpair = _rc_pairs["Front"];
    front_rcpair.render->setRenderTarget(new GMlib::RenderTexture("Front"));
    front_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
    front_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( front_rcpair.camera );
    front_rcpair.render->setViewport( 0, 0, init_viewport_size, init_viewport_size );

    // Side cam
    auto& side_rcpair = _rc_pairs["Side"];
    side_rcpair.render->setRenderTarget(new GMlib::RenderTexture("Side"));
    side_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
    side_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( side_rcpair.camera );
    side_rcpair.render->setViewport( 0, 0, init_viewport_size, init_viewport_size );

    // Top cam
    auto& top_rcpair = _rc_pairs["Top"];
    top_rcpair.render->setRenderTarget(new GMlib::RenderTexture("Top"));
    top_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
    top_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( top_rcpair.camera );
    top_rcpair.render->setViewport( 0, 0, init_viewport_size, init_viewport_size );























//    // Projection
//    _proj_cam = new GMlib::Camera( init_cam_pos, init_cam_dir, init_cam_up );
//    _proj_cam->set(init_cam_pos,init_cam_dir,init_cam_up);
//    _proj_cam->setCuttingPlanes( 1.0f, 8000.0f );
//    _proj_cam->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
//    _proj_cam->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );


//    // Front
//    _front_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
//    _front_cam->setCuttingPlanes( 1.0f, 8000.0f );
//    //  _front_cam->zoom( 5.0 );

//    // Side
//    _side_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
//    _side_cam->setCuttingPlanes( 1.0f, 8000.0f );
//    //  _side_cam->zoom( 5.0 );

//    // Up
//    _top_cam = new GMlib::Camera( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
//    _top_cam->setCuttingPlanes( 1.0f, 8000.0f );
//    //  _top_cam->zoom( 10.0 );

//    _scene->insertCamera( _proj_cam );
//    _scene->insertCamera( _front_cam );
//    _scene->insertCamera( _side_cam );
//    _scene->insertCamera( _top_cam );

//    _proj_renderer->setCamera(_proj_cam);
//    _front_renderer->setCamera(_front_cam);
//    _side_renderer->setCamera(_side_cam);
//    _top_renderer->setCamera(_top_cam);

//    _proj_renderer->setViewport(  0,0,init_viewport_size,init_viewport_size);
//    _front_renderer->setViewport( 0,0,init_viewport_size,init_viewport_size);
//    _side_renderer->setViewport(  0,0,init_viewport_size,init_viewport_size);
//    _top_renderer->setViewport(   0,0,init_viewport_size,init_viewport_size);

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

  } _context->doneCurrent();

}

GMlib::Scene*GMlibWrapper::getScene() const {

  return _scene;
}



void GMlibWrapper::select(int x, int y) {

  qDebug() << "Mouse clicked at " << x << " " << y;


  _context->makeCurrent(_offscreensurface ); {

    y = _proj_renderer->getViewportH() - y - 1;

    qDebug() << "Select: " << x << " " << y;

    _proj_renderer->select(-GMlib::GM_SO_TYPE_SELECTOR);

    GMlib::DisplayObject* so = _proj_renderer->findObject(x,y);
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
