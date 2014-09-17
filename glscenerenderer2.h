#ifndef GLSCENERENDERER2_H
#define GLSCENERENDERER2_H

#include <QQuickItem>

// gmlib
#include <opengl/gmtexture>
#include <opengl/gmprogram>
#include <opengl/shaders/gmvertexshader.h>
#include <opengl/shaders/gmfragmentshader.h>
#include <opengl/bufferobjects/gmvertexbufferobject.h>

// qt
#include <QOpenGLShaderProgram>
#include <QDebug>


// stl
#include <memory>


namespace Private {

  class Renderer: public QObject {
    Q_OBJECT
  public:
    Renderer( const std::string& tex_name ) { _tex.acquire(tex_name); }

    void      setTexName( const std::string& name ) { _tex.acquire(name); }

  public slots:
    void      paint() {
      qDebug() << "Painting on viewport: " << _viewport << ", tex id: " << _tex.getId();

      if( !_prog.isValid() ) {
        std::cout << "Prog ! valid: setting up." << std::endl;


        _vs.create();
        _fs.create();
        _prog.create();

        _vs.setSource(
              "#version 150 compatibility\n"
              "layout(std140) uniform;\n"
              "in  vec4 vertices;"
              "out vec2 coords;"
              "out vec4 gl_Position;"
              "void main() {"
              "    gl_Position = vertices;"
              "    coords = (vertices.xy + vec2(1.0,1.0))*0.5;"
              "}"
              );

        _fs.setSource(
              "#version 150 compatibility\n"
              "layout(std140) uniform;\n"
              "\n"
              "uniform sampler2D u_tex0;"
              "in vec2 coords;"
              "void main() {"
              "    gl_FragColor = texture( u_tex0, coords.st );"
              "}"
              );


        if( !_vs.compile() ) {
          std::cout << "Vertex shader compile error: " << _vs.getCompilerLog() << std::endl;
          exit(-666);
        }
        if(!_fs.compile()) {
          std::cout << "Fragment shader compile error: " << _fs.getCompilerLog() << std::endl;
          exit(-666);
        }
        _prog.attachShader(_vs);
        _prog.attachShader(_fs);
        if( _prog.link() != GL_TRUE ) {
          std::cout << "Render prog link error: " << _prog.getLinkerLog() << std::endl;
          exit(-666);
        }


        const GLfloat values[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        _vbo.create();
        _vbo.bufferData( 2 * 4 * sizeof(GLfloat), values, GL_STATIC_DRAW );
      }


      std::cout << "Prog OK!" << std::endl;




      glViewport(_viewport.x(), _viewport.y(), _viewport.width(), _viewport.height());

      glDisable(GL_DEPTH_TEST);

//      glClearColor(0, 0, 0, 1);
//      glClear(GL_COLOR_BUFFER_BIT);

      glDisable(GL_BLEND);
//      glEnable(GL_BLEND);
//      glBlendFunc(GL_SRC_ALPHA, GL_ONE);




      _prog.bind(); {


        _prog.setUniform( "u_tex0", _tex, GL_TEXTURE0, GLuint(0) );

        GMlib::GL::AttributeLocation vert_loc = _prog.getAttributeLocation("vertices");

        _vbo.bind();
        _vbo.enableVertexArrayPointer( vert_loc, 2, GL_FLOAT, GL_FALSE, 0, static_cast<const GLvoid*>(nullptr) );
        GL_CHECK(::glDrawArrays(GL_TRIANGLE_STRIP,0,4));
        _vbo.disable(vert_loc);
        _vbo.unbind();


      } _prog.unbind();






//      if( !_prog ) {

//        _prog = std::unique_ptr<QOpenGLShaderProgram>( new QOpenGLShaderProgram );
//        _prog->addShaderFromSourceCode(QOpenGLShader::Vertex,
//                                                   "attribute highp vec4 vertices;"
//                                                   "varying highp vec2 coords;"
//                                                   "void main() {"
//                                                   "    gl_Position = vertices;"
//                                                   "    coords = vertices.xy;"
//                                                   "}");
//          _prog->addShaderFromSourceCode(QOpenGLShader::Fragment,
//                                             "uniform lowp float t;"
//                                             "varying highp vec2 coords;"
//                                             "void main() {"
//                                             "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
//                                             "    i = smoothstep(t - 0.8, t + 0.8, i);"
//                                             "    i = floor(i * 20.) / 20.;"
//                                             "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
//                                             "}");

//          _prog->bindAttributeLocation("vertices", 0);
//          _prog->link();


//      }
//      _prog->bind();

//      _prog->enableAttributeArray(0);

//      const float values[] = {
//          -1, -1,
//          1, -1,
//          -1, 1,
//          1, 1
//      };
//      _prog->setAttributeArray(0, GL_FLOAT, values, 2);
//      _prog->setUniformValue("t", 0.5f);

//      glViewport(_viewport.x(), _viewport.y(), _viewport.width(), _viewport.height());

//      glDisable(GL_DEPTH_TEST);

////      glClearColor(0, 0, 0, 1);
////      glClear(GL_COLOR_BUFFER_BIT);

//      glDisable(GL_BLEND);
////      glEnable(GL_BLEND);
////      glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//      _prog->disableAttributeArray(0);
//      _prog->release();




    }
    void      setViewport( const QRectF& rect ) { _viewport = rect; }

  private:
    QRectF                          _viewport;

    GMlib::GL::VertexBufferObject   _vbo;
    GMlib::GL::Texture              _tex;

    GMlib::GL::Program              _prog;
    GMlib::GL::VertexShader         _vs;
    GMlib::GL::FragmentShader       _fs;

//    std::unique_ptr<QOpenGLShaderProgram>   _prog;

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
