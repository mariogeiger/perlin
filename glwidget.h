#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QTime>
#include "perlinnoise2d.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
    
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void timerEvent(QTimerEvent *);
    void mouseMoveEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    PerlinNoise2D *noise;

    QTime time;
    int keys;

    float alpha;
    float beta;
    QPointF position;
};

#endif // GLWIDGET_H
