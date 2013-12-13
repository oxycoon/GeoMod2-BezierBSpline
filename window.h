#ifndef WINDOW_H
#define WINDOW_H

#include <QQuickView>

class Window : public QQuickView {
  Q_OBJECT
public:
  explicit Window(QWindow *parent = 0);

signals:
  void      signSceneRenderGeometryChanged( const QString& name, const QRectF& new_geometry );
  void      signFrameReady();
};

#endif // WINDOW_H
