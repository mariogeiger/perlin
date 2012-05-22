#ifndef PERLINNOISE2D_H
#define PERLINNOISE2D_H

#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLShaderProgram>
#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

class PerlinNoise2D : protected QGLFunctions
{
public:
    PerlinNoise2D(float _width, float _height, int octave, float step, float persistance, const QGLContext *context);
    ~PerlinNoise2D();
    void setProjection(const QMatrix4x4 &matrix);
    void setView(const QMatrix4x4 &matrix);
    void draw();
    void init();
    float altitude(float x, float y);
    float width() const;
    float height() const;

private:
    float interp1d(float a, float b, float x);
    float interp2d(float a, float b, float c, float d, float x, float y);
    float noise(float x, float y);

    QVector<QVector3D> vertices;
    QVector<GLfloat> randoms;

    QGLBuffer vbo;
    QGLBuffer rbo;
    QGLBuffer ibo;

    QGLShaderProgram *program;
    int vertexLocation;
    int matrixProjectionLocation;
    int matrixViewLocation;

    float _width, _height;
    int w, h;
    int octave;
    float step;
    float persistance;
};

#endif // PERLINNOISE2D_H
