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

// stl
#include <thread>
#include <mutex>





namespace Private {

  class CurrentContextLock{
  public:
    explicit CurrentContextLock( const std::shared_ptr<QOpenGLContext>& context,
                             const std::shared_ptr<QOffscreenSurface>& surface )
      : _context{context}, _surface{surface}, _mutex{} {

      _mutex.lock();
      _context->makeCurrent(_surface.get());
    }

    ~CurrentContextLock() {
      _mutex.unlock();
    }

    void    done() { _context->doneCurrent(); _mutex.unlock(); }
    void    makeCurrent() { _mutex.lock(); _context->makeCurrent(_surface.get()); }

  private:
    std::shared_ptr<QOpenGLContext>       _context;
    std::shared_ptr<QOffscreenSurface>    _surface;
    std::mutex                            _mutex;
  };
}






std::unique_ptr<GMlibWrapper> GMlibWrapper::_instance {nullptr};




GMlibWrapper::GMlibWrapper(QOpenGLContext *top_context)
//  : GMlibWrapper()
  : QObject(), _timer_id{0}
{

  if(_instance != nullptr) {

    std::cerr << "This version of the GMlibWrapper only supports a single instance of the GMlibWraper..." << std::endl;
    std::cerr << "Only one of me(0x" << this << ") please!! ^^" << std::endl;
    assert(!_instance);
    exit(666);
  }


  _instance = std::unique_ptr<GMlibWrapper>(this);

  // Create Internal shared GL context
  _context = std::make_shared<QOpenGLContext>();
  _context->setShareContext( top_context );
  _context->setFormat( top_context->format() );
  _context->create();

  // Set up offscreen rendering surface for GMlib rendering
  _offscreensurface = std::make_shared<QOffscreenSurface>();
  _offscreensurface->setFormat(_context->format());
  _offscreensurface->create();


  Private::CurrentContextLock ccxt(_context,_offscreensurface);

  // Setup and initialized GMlib GL backend
  GMlib::GL::OpenGLManager::init();

  // Setup and init the GMlib GMWindow
  _scene = std::make_shared<GMlib::Scene>();
}

GMlibWrapper::~GMlibWrapper() {

  stop();
}

void GMlibWrapper::changeRenderGeometry(const QString& name, const QRectF& geometry) {

  const QSize size = geometry.size().toSize();

  if( size.width() <= 0 || size.height() <= 0 )
    return;

  if( _rc_pairs.count(name.toStdString()) <= 0 )
    return;

  auto& rc_pair = _rc_pairs[name.toStdString()];
  if(rc_pair.viewport.geometry == geometry )
    return;

  rc_pair.viewport.geometry = geometry;
  rc_pair.viewport.changed = true;
}

void GMlibWrapper::timerEvent(QTimerEvent* e) {




  if( !_context->isValid() )
    return;

  e->accept();



  // Simuation order
  // 1) Prepare must be run first
  // 2) Simulate and render can be run in parallell


  // Grab and activate GL context
  Private::CurrentContextLock cctx(_context,_offscreensurface);

  // 1)
  _scene->prepare();

  std::vector<std::thread> threads;

  // Add simulation thread
  threads.push_back(std::thread(&GMlib::Scene::simulate,_scene));

  // Add Render threads
  for( auto& rc_pair : _rc_pairs ) {
//      qDebug() << "About to render: " << rc_pair.first.c_str();
//      qDebug() << "  Viewport: ";
//      qDebug() << "    Changed: " << rc_pair.second.viewport.changed;
//      qDebug() << "    Geometry: " << rc_pair.second.viewport.geometry;

    if(rc_pair.second.viewport.changed) {
      const QSizeF size = rc_pair.second.viewport.geometry.size();
      rc_pair.second.render->reshape( GMlib::Vector<int,2>(size.width(),size.height()));
      rc_pair.second.viewport.changed = false;
    }


    rc_pair.second.render->render();
    rc_pair.second.render->swap();


    //// THIS DOES NOT WORK AND WE KNOW WHY: need to move context around !!!!
//      threads.push_back(std::thread(&GMlib::Renderer::render,rc_pair.second.render));
//      threads.push_back(std::thread(&GMlib::DefaultRendererWithSelect::render,rc_pair.second.render));

//      Private::renderCaller(rc_pair.second.render,_context,_offscreensurface);
//      threads.push_back(std::thread(&Private::renderCaller,rc_pair.second.render,_context,_offscreensurface));
  }

  for( auto& thread : threads )
    thread.join();





  cctx.done();

  emit signFrameReady();
}

void GMlibWrapper::moveObj(const GMlib::Vector<float,2>& dir) {

  _obj_pos += dir;

  if( _obj_pos(0) > 1.0 ) _obj_pos[0] -= 1.0;
  if( _obj_pos(1) > 1.0 ) _obj_pos[1] -= 1.0;

  double u, v;
  u = _world->getParStartU() + _world->getParDeltaU() * _obj_pos(0);
  v = _world->getParStartV() + _world->getParDeltaV() * _obj_pos(1);

  _context->makeCurrent(_offscreensurface.get()); {

    GMlib::DMatrix< GMlib::Vector<float,3> > w_eval = _world->evaluateGlobal( u, v, 1, 1 );
//    _obj->translateGlobal( (w_eval(0)(0) - _obj->getPos()) );

    GMlib::Vector<float,3> n_corr = w_eval(1)(0) ^ w_eval(0)(1);
    n_corr.normalize();
    n_corr *= _obj->getSurroundingSphere().getRadius();
    _obj->translateGlobal(n_corr + (w_eval(0)(0) - _obj->getPos()) );

  } _context->doneCurrent();
}

const GMlibWrapper&
GMlibWrapper::getInstance() {
  return *_instance;
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

void GMlibWrapper::initTestScene() {

  // Make OpenGL context current on offscreensurface
  Private::CurrentContextLock ccxt(_context,_offscreensurface);

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

    rcpair.second.render = new GMlib::DefaultRenderer;
    rcpair.second.camera = new GMlib::Camera;
    rcpair.second.render->setCamera(rcpair.second.camera);
  }

  // Projection cam
  auto& proj_rcpair = _rc_pairs["Projection"];
  proj_rcpair.camera->set(init_cam_pos,init_cam_dir,init_cam_up);
  proj_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
  proj_rcpair.camera->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
  proj_rcpair.camera->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );
  _scene->insertCamera( proj_rcpair.camera );
  proj_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

  // Front cam
  auto& front_rcpair = _rc_pairs["Front"];
  front_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
  front_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
  _scene->insertCamera( front_rcpair.camera );
  front_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

  // Side cam
  auto& side_rcpair = _rc_pairs["Side"];
  side_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
  side_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
  _scene->insertCamera( side_rcpair.camera );
  side_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

  // Top cam
  auto& top_rcpair = _rc_pairs["Top"];
  top_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
  top_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
  _scene->insertCamera( top_rcpair.camera );
  top_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );


  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );

//  _world = new GMlib::PTorus<float>();
  _world = std::make_shared<TestTorus>();
  _world->toggleDefaultVisualizer();
  _world->replot( 200, 200, 1, 1 );
  _scene->insert(_world.get());

  _obj = std::make_shared<GMlib::PSphere<float>,float>(2.0f);
  _obj->toggleDefaultVisualizer();
  _obj->replot( 200, 200, 1, 1 );
  _world->insert(_obj.get());

  _obj_pos = GMlib::Vector<float,2>( 0.0f, 0.0f );

  moveObj( GMlib::Vector<float,2>(0.0f,0.0f) );
}

const std::shared_ptr<GMlib::Scene>&
GMlibWrapper::getScene() const {

  return _scene;
}

const GMlib::TextureRenderTarget&
GMlibWrapper::getRenderTextureOf(const std::string& name) const {

  assert(_rc_pairs.count(name));
  return _rc_pairs.at(name).render->getFrontRenderTarget();
}

