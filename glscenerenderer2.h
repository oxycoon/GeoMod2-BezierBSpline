#ifndef GLSCENERENDERER2_H
#define GLSCENERENDERER2_H

#include <QQuickItem>

// gmlib
#include <opengl/gmtexture.h>

// qt
#include <QOpenGLShaderProgram>
#include <QDebug>


// stl
#include <memory>


namespace Private {

  class Renderer: public QObject {
    Q_OBJECT
  public:
    Renderer( const std::string& tex_name ) : _prog(nullptr) { _tex.acquire(tex_name); }

    void      setTexName( const std::string& name ) { _tex.acquire(name); }

  public slots:
    void      paint() {
      qDebug() << "Painting on viewport: " << _viewport << ", tex id: " << _tex.getId();

      if( !_prog ) {

        _prog = std::unique_ptr<QOpenGLShaderProgram>( new QOpenGLShaderProgram );
        _prog->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                   "attribute highp vec4 vertices;"
                                                   "varying highp vec2 coords;"
                                                   "void main() {"
                                                   "    gl_Position = vertices;"
                                                   "    coords = vertices.xy;"
                                                   "}");
          _prog->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             "uniform lowp float t;"
                                             "varying highp vec2 coords;"
                                             "void main() {"
                                             "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
                                             "    i = smoothstep(t - 0.8, t + 0.8, i);"
                                             "    i = floor(i * 20.) / 20.;"
                                             "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
                                             "}");

          _prog->bindAttributeLocation("vertices", 0);
          _prog->link();


      }
      _prog->bind();

      _prog->enableAttributeArray(0);

      const float values[] = {
          -1, -1,
          1, -1,
          -1, 1,
          1, 1
      };
      _prog->setAttributeArray(0, GL_FLOAT, values, 2);
      _prog->setUniformValue("t", 0.5f);

      glViewport(_viewport.x(), _viewport.y(), _viewport.width(), _viewport.height());

      glDisable(GL_DEPTH_TEST);

//      glClearColor(0, 0, 0, 1);
//      glClear(GL_COLOR_BUFFER_BIT);

      glDisable(GL_BLEND);
//      glEnable(GL_BLEND);
//      glBlendFunc(GL_SRC_ALPHA, GL_ONE);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      _prog->disableAttributeArray(0);
      _prog->release();




    }
    void      setViewport( const QRectF& rect ) { _viewport = rect; }

  private:
    QRectF                _viewport;
    GMlib::GL::Texture    _tex;

    std::unique_ptr<QOpenGLShaderProgram>   _prog;

  }; // END namespace Renderer

}


class GLSceneRenderer2 : public QQuickItem {
  Q_OBJECT
  Q_PROPERTY(QString name     READ getTexName WRITE setTexName)
  Q_PROPERTY(bool    paused   READ isPaused   WRITE setPaused)

public:
  explicit GLSceneRenderer2();

  const QString&        getTexName() const;
  void                  setTexName( const QString& tex_name );

  bool                  isPaused() const;
  void                  setPaused( bool paused );

public slots:
  void      sync();
  void      cleanup();


protected:
  void      geometryChanged(const QRectF &newGeometry, const QRectF&) {

    qDebug() << "Geometry changed of: " << _tex_name << ", geo: " << mapRectToScene(newGeometry);
  }

private slots:
  void      handleWindowChanged( QQuickWindow * window );

private:
  std::unique_ptr<Private::Renderer>      _renderer;
  QString                                 _tex_name;
  bool                                    _paused;


};



#endif // GLSCENERENDERER2_H
