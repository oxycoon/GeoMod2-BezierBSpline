#include "gmlibwrapper.h"


#include "testtorus.h"
#include "utils.h"


// GMlib
#include <gmOpenglModule>
#include <gmSceneModule>
#include <gmParametricsModule>

// Qt
#include <QTimerEvent>
#include <QRectF>
#include <QDebug>

// stl
#include <stdexcept>
#include <thread>
#include <mutex>





std::unique_ptr<GMlibWrapper> GMlibWrapper::_instance {nullptr};


GMlibWrapper::GMlibWrapper(QOpenGLContext *context)
//  : GMlibWrapper()
  : QObject(), _timer_id{0}, _glsurface{context}, _select_renderer{nullptr}
{

  if(_instance != nullptr) {

    std::cerr << "This version of the GMlibWrapper only supports a single instance of the GMlibWraper..." << std::endl;
    std::cerr << "Only one of me(0x" << this << ") please!! ^^" << std::endl;
    assert(!_instance);
    exit(666);
  }


  _instance = std::unique_ptr<GMlibWrapper>(this);

  _glsurface.makeCurrent();

  // Setup and initialized GMlib GL backend
  GMlib::GL::OpenGLManager::init();

  // Setup and init the GMlib GMWindow
  _scene = std::make_shared<GMlib::Scene>();
}

GMlibWrapper::~GMlibWrapper() {

  stop();

  _scene->clear();

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

void GMlibWrapper::mousePressed(const QString& name, const QPointF& pos) {


  const auto& rc_select = _rc_pairs.at(name.toStdString());
  const auto& rc_geo = rc_select.viewport.geometry;



  GMlib::Vector<int,2> size(rc_geo.width(),rc_geo.height());
  _select_renderer->setCamera(rc_select.camera);

  GMlib::DisplayObject* obj = {nullptr};
  _glsurface.makeCurrent(); {

    _select_renderer->reshape( size );
    _select_renderer->select( 0 );

    _select_renderer->prepare();

    obj = _select_renderer->findObject(pos.x(),size(1)-pos.y()-1);

  } _glsurface.doneCurrent();

  if(obj) obj->toggleSelected();
}

void GMlibWrapper::timerEvent(QTimerEvent* e) {

  e->accept();

  // Simuation order
  // 1) Prepare must be run first
  // 2) Simulate and render can be run in parallell


  // Grab and activate GL context
  _glsurface.makeCurrent(); {

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
    }

    for( auto& thread : threads )
      thread.join();

  } _glsurface.doneCurrent();

  emit signFrameReady();
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

void GMlibWrapper::initScene() {

  // Make OpenGL context current on offscreensurface
  _glsurface.makeCurrent(); {

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


    // Setup Select Renderer
    _select_renderer = std::make_shared<GMlib::DefaultSelectRenderer>();


    // My test torus
    _torus = std::make_shared<TestTorus>();
    _torus->toggleDefaultVisualizer();
    _torus->replot( 200, 200, 1, 1 );
    _scene->insert(_torus.get());

  } _glsurface.doneCurrent();
}

const std::shared_ptr<GMlib::Scene>&
GMlibWrapper::getScene() const {

  return _scene;
}

const GMlib::TextureRenderTarget&
GMlibWrapper::getRenderTextureOf(const std::string& name) const {

  if(!_rc_pairs.count(name)) throw std::invalid_argument("[][]Render/Camera pair '" + name + "'  does not exist!");

  return _rc_pairs.at(name).render->getFrontRenderTarget();
}

