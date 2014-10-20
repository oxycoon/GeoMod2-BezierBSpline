#ifndef WINDOW_H
#define WINDOW_H

#include <QQuickView>

class Window : public QQuickView {
  Q_OBJECT
public:
  explicit Window(QWindow *parent = 0);

signals:
  void      signGuiViewportChanged( const QString& name, const QRectF& new_geometry );
  void      signFrameReady();

  // Relay singals from qml side
  void      signMousePressed( const QString& render_name, const QPointF& pos );
};

#endif // WINDOW_H
