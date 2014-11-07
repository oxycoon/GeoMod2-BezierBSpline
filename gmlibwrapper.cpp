#include "gmlibwrapper.h"
#include "glcontextsurfacewrapper.h"


#include "testtorus.h"
#include "bezierbspline.h"
#include "utils.h"


// GMlib
#include <gmOpenglModule>
#include <gmSceneModule>
#include <gmParametricsModule>

// Qt
#include <QTimerEvent>
#include <QRectF>
#include <QMouseEvent>
#include <QDebug>

// stl
#include <stdexcept>
#include <thread>
#include <mutex>





std::unique_ptr<GMlibWrapper> GMlibWrapper::_instance {nullptr};


GMlibWrapper::GMlibWrapper(std::shared_ptr<GLContextSurfaceWrapper> context)
//  : GMlibWrapper()
  : QObject(), _timer_id{0}, _glsurface(context), _select_renderer{nullptr}
{

  if(_instance != nullptr) {

    std::cerr << "This version of the GMlibWrapper only supports a single instance of the GMlibWraper..." << std::endl;
    std::cerr << "Only one of me(0x" << this << ") please!! ^^" << std::endl;
    assert(!_instance);
    exit(666);
  }


  _instance = std::unique_ptr<GMlibWrapper>(this);

  _glsurface->makeCurrent();

  // Setup and initialized GMlib GL backend
  GMlib::GL::OpenGLManager::init();

  // Setup and init the GMlib GMWindow
  _scene = std::make_shared<GMlib::Scene>();
}

GMlibWrapper::~GMlibWrapper() {

  stop();

  _glsurface->makeCurrent(); {

    _select_renderer->releaseCamera();
    _select_renderer.reset();

    for( auto& rc_pair : _rc_pairs ) {

      rc_pair.second.render->releaseCamera();
      _scene->removeCamera( rc_pair.second.camera.get() );

      rc_pair.second.render.reset();
      rc_pair.second.camera.reset();
    }

    _scene->clear();

  } _glsurface->doneCurrent();
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

  e->accept();

  // Simuation order
  // 1) Prepare must be run first
  // 2) Simulate and render can be run in parallell


  // Grab and activate GL context
  _glsurface->makeCurrent(); {

    // 1)
    _scene->prepare();


    _scene->simulate();

//    std::vector<std::thread> threads;

    // Add simulation thread
//    threads.push_back(std::thread(&GMlib::Scene::simulate,_scene));

    // Add Render threads
    for( auto& rc_pair : _rc_pairs ) {
  //      qDebug() << "About to render: " << rc_pair.first.c_str();
  //      qDebug() << "  Viewport: ";
  //      qDebug() << "    Changed: " << rc_pair.second.viewport.changed;
  //      qDebug() << "    Geometry: " << rc_pair.second.viewport.geometry;

      if(rc_pair.second.viewport.changed) {
        const QSizeF size = rc_pair.second.viewport.geometry.size();
        rc_pair.second.render->reshape( GMlib::Vector<int,2>(size.width(),size.height()));
        rc_pair.second.camera->reshape( 0, 0, size.width(), size.height() );
        rc_pair.second.viewport.changed = false;
      }

      rc_pair.second.render->render();
      rc_pair.second.render->swap();
    }

//    for( auto& thread : threads )
//      thread.join();

  } _glsurface->doneCurrent();

  emit signFrameReady();
}

const GMlibWrapper&
GMlibWrapper::getInstance() {
  return *_instance;
}

void GMlibWrapper::start() {

  if( _timer_id || _scene->isRunning() )
    return;

  _timer_id = startTimer(16, Qt::PreciseTimer);
  _scene->start();
}

void GMlibWrapper::stop() {

  if( !_timer_id || !_scene->isRunning() )
    return;

  _scene->stop();
  killTimer(_timer_id);
  _timer_id = 0;
}

void GMlibWrapper::initScene() {

  // Make OpenGL context current on offscreensurface
  _glsurface->makeCurrent(); {

    // Insert a light
    GMlib::Point<GLfloat,3> init_light_pos( 2.0, 4.0, 10 );
    GMlib::PointLight *light = new GMlib::PointLight(  GMlib::GMcolor::White, GMlib::GMcolor::White,
                                                       GMlib::GMcolor::White, init_light_pos );
    light->setAttenuation(0.8, 0.002, 0.0008);
    _scene->insertLight( light, false );

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

      rcpair.second.render = std::make_shared<GMlib::DefaultRenderer>();
      rcpair.second.camera = std::make_shared<GMlib::Camera>();
      rcpair.second.render->setCamera(rcpair.second.camera.get());
    }

    // Projection cam
    auto& proj_rcpair = _rc_pairs["Projection"];
    proj_rcpair.camera->set(init_cam_pos,init_cam_dir,init_cam_up);
    proj_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    proj_rcpair.camera->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
    proj_rcpair.camera->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );
    _scene->insertCamera( proj_rcpair.camera.get() );
    proj_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

    // Front cam
    auto& front_rcpair = _rc_pairs["Front"];
    front_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, -50.0f, 0.0f ), init_cam_dir, init_cam_up );
    front_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( front_rcpair.camera.get() );
    front_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

    // Side cam
    auto& side_rcpair = _rc_pairs["Side"];
    side_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( -50.0f, 0.0f, 0.0f ), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ), init_cam_up );
    side_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( side_rcpair.camera.get() );
    side_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );

    // Top cam
    auto& top_rcpair = _rc_pairs["Top"];
    top_rcpair.camera->set( init_cam_pos + GMlib::Vector<float,3>( 0.0f, 0.0f, 50.0f ), -init_cam_up, init_cam_dir );
    top_rcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
    _scene->insertCamera( top_rcpair.camera.get() );
    top_rcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );





//    // Iso Camera
//    auto& isorcpair = (_rc_pairs["Iso"] = RenderCamPair {});
//    isorcpair.render = std::make_shared<GMlib::DefaultRenderer>();
//    isorcpair.camera = std::make_shared<GMlib::IsoCamera>();
//    isorcpair.render->setCamera(isorcpair.camera.get());
//    _scene->insertCamera( isorcpair.camera.get() );
//    isorcpair.camera->set(init_cam_pos,init_cam_dir,init_cam_up);
//    isorcpair.camera->setCuttingPlanes( 1.0f, 8000.0f );
//    isorcpair.camera->rotateGlobal( GMlib::Angle(-45), GMlib::Vector<float,3>( 1.0f, 0.0f, 0.0f ) );
//    isorcpair.camera->translate( GMlib::Vector<float,3>( 0.0f, -20.0f, 20.0f ) );
//    isorcpair.render->reshape( GMlib::Vector<int,2>(init_viewport_size, init_viewport_size) );


    // Setup Select Renderer
    _select_renderer = std::make_shared<GMlib::DefaultSelectRenderer>();



//#define TEST_CURVE
#define TEST_SURFACE


#ifdef TEST_CURVE

    // Curve visualizers
    auto curve_visualizer = new GMlib::PCurveDerivativesVisualizer<float,3>;

    // Curve
    auto curve = new GMlib::PCircle<float>(2.0f);
//    curve->toggleDefaultVisualizer();
    curve->insertVisualizer(curve_visualizer);
    curve->replot(100,1);
    _scene->insert(curve);

#endif





#ifdef TEST_SURFACE

    // Surface visualizers
//    auto surface_visualizer = new GMlib::PSurfDerivativesVisualizer<float,3>;
//    auto surface_visualizer = new GMlib::PSurfNormalsVisualizer<float,3>;
//    auto surface_visualizer = new GMlib::PSurfParamLinesVisualizer<float,3>;
    auto surface_visualizer = new GMlib::PSurfPointsVisualizer<float,3>;

    // Surface
    auto surface = new TestTorus;
//    surface->toggleDefaultVisualizer();
    surface->insertVisualizer(surface_visualizer);
    surface->replot(200,200,1,1);
    _scene->insert(surface);

    surface->test01();

#endif

    BezierBSpline<float>* spline;// = new BezierBSpline<float>();
    GMlib::DMatrix<float> temp;
    spline->evaluate(temp, 3, 0.7f, 1.0f);


  } _glsurface->doneCurrent();
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

void
GMlibWrapper::mousePressed(const QString& name, QMouseEvent* event ) {

  const QPointF& pos = event->pos();

  const auto& rc_select = _rc_pairs.at(name.toStdString());
  const auto& rc_geo = rc_select.viewport.geometry;

  GMlib::Vector<int,2> size(rc_geo.width(),rc_geo.height());
  _select_renderer->setCamera(rc_select.camera.get());

  GMlib::SceneObject* obj = {nullptr};
  _glsurface->makeCurrent(); {

    _select_renderer->reshape( size );
    _select_renderer->select( 0 );

    _select_renderer->prepare();

    obj = _select_renderer->findObject(pos.x(),size(1)-pos.y()-1);

  } _glsurface->doneCurrent();

  if(obj) obj->toggleSelected();
}

void GMlibWrapper::mouseReleased(const QString& name, QMouseEvent* event) {
  Q_UNUSED(name)
  Q_UNUSED(event)

  qDebug() << "MouseReleased";
}

void GMlibWrapper::mouseDoubleClicked(const QString& name, QMouseEvent* event) {
  Q_UNUSED(name)
  Q_UNUSED(event)

  qDebug() << "MouseDoubleClicked";
}

void
GMlibWrapper::keyPressed(const QString& name, QKeyEvent* event) {
  Q_UNUSED(name)

  if( event->key() == Qt::Key_R ) _scene->toggleRun();
}

void GMlibWrapper::keyReleased(const QString& name, QKeyEvent* event) {
  Q_UNUSED(name)
  Q_UNUSED(event)

  qDebug() << "KeyReleased";
}

void GMlibWrapper::wheelEventOccurred(const QString& name, QWheelEvent* event) {

  int delta = event->delta();
  auto& rc = _rc_pairs.at(name.toStdString());

  const auto& camera_geo = rc.viewport.geometry;
  auto camera    = rc.camera.get();
  auto isocamera = dynamic_cast<GMlib::IsoCamera*>(camera);
  if(isocamera) {

    if( delta < 0 ) isocamera->zoom( 1.05 );
    if( delta > 0 ) isocamera->zoom( 0.95 );
  }
  else if(camera) {

    double scale;
    if( camera->isLocked() )
      scale = camera->getLockDist();
    else
      scale = getScene()->getSphere().getRadius();

    camera->move( delta*scale / camera_geo.height());

  }

}

