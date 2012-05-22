#include "glwidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      noise(0), keys(0), alpha(0.0), beta(0.0)
{
    setWindowState(windowState() | Qt::WindowFullScreen);

    setCursor(QCursor(Qt::BlankCursor));
    QCursor::setPos(geometry().center());

    setMouseTracking(true);
    time.start();
    startTimer(0);
}

GLWidget::~GLWidget()
{
    if (noise) delete noise;
}

void GLWidget::initializeGL()
{
    noise = new PerlinNoise2D(30.0, 30.0, 5, 3.0, 0.5, context());
    position = QPointF(noise->width(), noise->height()) / 2.0;
    noise->init();

    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    QMatrix4x4 m;
    m.perspective(70.0, float(w)/float(h?h:1), 0.01, 1000.0);
    noise->setProjection(m);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 m;
    float h = 0.04;
    m.rotate(-90.0, 1, 0, 0);
    m.rotate(beta, 1, 0, 0);
    m.rotate(alpha, 0, 0, 1);
    m.translate(-position.x(), -position.y(), -(noise->altitude(position.x(), position.y()) + h));
    noise->setView(m);
    noise->draw();
}

void GLWidget::timerEvent(QTimerEvent *)
{
    float dt = float(time.restart()) / 1000.0;

    QPointF front(0, 0.5);
    QTransform r;
    r.rotate(-alpha);
    front = r.map(front);
    r.reset();
    r.rotate(-90);
    QPointF right = r.map(front);

    if (keys & 0x1) {
        position += front * dt;
    }
    if (keys & 0x2) {
        position -= front * dt;
    }
    if (keys & 0x4) {
        position += right * dt;
    }
    if (keys & 0x8) {
        position -= right * dt;
    }

    if (position.x() < 0.0) {
        position.setX(0.0);
    }
    if (position.y() < 0.0) {
        position.setY(0.0);
    }
    if (position.x() > noise->width()) {
        position.setX(noise->width());
    }
    if (position.y() > noise->height()) {
        position.setY(noise->height());
    }

    updateGL();
}

float clamp(float min, float val, float max)
{
    return val<min ? min : (val>max ? max : val);
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint center = geometry().center();
    QPointF move = e->globalPos() - center;

    alpha += move.x();
    beta += move.y();
    beta = clamp(-90, beta, 90);

    QCursor::setPos(center);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        keys |= 0x1;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        keys |= 0x2;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        keys |= 0x4;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        keys |= 0x8;
        break;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        keys &= ~0x1;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        keys &= ~0x2;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        keys &= ~0x4;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        keys &= ~0x8;
        break;
    }
}
