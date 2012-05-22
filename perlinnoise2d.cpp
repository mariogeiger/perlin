#include "perlinnoise2d.h"
#include <QDebug>
#include <QFile>
#include <cmath>
#define BUFFER_OFFSET(a) ((char*)NULL + (a))

PerlinNoise2D::PerlinNoise2D(float width, float height, int octave, float step, float persistance, const QGLContext *context) :
    vbo(QGLBuffer::VertexBuffer),
    rbo(QGLBuffer::VertexBuffer),
    ibo(QGLBuffer::IndexBuffer),
    _width(width), _height(height),
    octave(octave), step(step), persistance(persistance)
{
    initializeGLFunctions(context);

    /*

      <-----w----->
      <-s-><-s-><-s->

      littlest step = step / 2^(octave-1)

      size = ceil( width / (step / 2^(octave-1)))
      size = ceil( width * 2^(octave-1) / step)
      size = ceil( width * (1 << (octave-1)) / step)

     */

    w = std::ceil(width * float(1 << (octave - 1)) / step) + 1;
    h = std::ceil(height * float(1 << (octave - 1)) / step) + 1;

    /* /\
     y |
       |...
       |5 6 7 8 9
       |0 1 2 3 4
       +-----------> x
     */

    vertices.reserve(w*h);
    float dx = width / float(w-1);
    float dy = height / float(h-1);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            vertices << QVector3D(dx * x, dy * y, 0.0);
        }
    }
    vbo.create();
    vbo.bind();
    vbo.allocate(vertices.size() * sizeof (GLfloat) * 3);

    program = new QGLShaderProgram(context);
    program->addShaderFromSourceFile(QGLShader::Vertex, ":/shader/perlinnoise2d.vert");
    program->addShaderFromSourceFile(QGLShader::Fragment, ":/shader/perlinnoise2d.frag");
    if (!program->link())
        qDebug() << program->log();

    program->bind();

    vertexLocation = program->attributeLocation("vertex");
    matrixProjectionLocation = program->uniformLocation("matrixp");
    matrixViewLocation = program->uniformLocation("matrixv");

    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3);
    vbo.release();

    QMatrix4x4 m;
    program->setUniformValue("matrixm", m);
    program->release();

    /*
      ...
      5 6 7 8 9
      0 1 2 3 4

      GL_CCW
      GL_TRIANGLE_STRIP --> 5, 0, 6, 1, 7, 2...
     */
    QVector<GLuint> indices;
    indices.reserve(2 * w * (h-1));
    for (int y = 1; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            indices << y * w + x;
            indices << (y-1) * w + x;
        }
    }
    ibo.create();
    ibo.bind();
    ibo.allocate(indices.constData(), indices.size() * sizeof (GLuint));
    ibo.release();
}

PerlinNoise2D::~PerlinNoise2D()
{
    delete program;
}

void PerlinNoise2D::setProjection(const QMatrix4x4 &matrix)
{
    program->bind();
    program->setUniformValue(matrixProjectionLocation, matrix);
    program->release();
}

void PerlinNoise2D::setView(const QMatrix4x4 &matrix)
{
    program->bind();
    program->setUniformValue(matrixViewLocation, matrix);
    program->release();
}

void PerlinNoise2D::draw()
{
    program->bind();
    program->enableAttributeArray(vertexLocation);

    ibo.bind();
    for (int y = 0; y < h-1; ++y) {
        glDrawElements(GL_TRIANGLE_STRIP, 2*w, GL_UNSIGNED_INT, BUFFER_OFFSET(y * 2*w * sizeof (GLuint)));
    }
    ibo.release();

    program->disableAttributeArray(vertexLocation);
    program->release();
}

void PerlinNoise2D::init()
{
    randoms.resize((w+1)*(h+1));
    for (int i = 0; i < randoms.size(); ++i) {
        randoms[i] = double(qrand()) / double(RAND_MAX);
    }

    for (int i = 0; i < vertices.size(); ++i) {
        vertices[i].setZ(altitude(vertices[i].x(), vertices[i].y()));
    }

    vbo.bind();
    vbo.write(0, vertices.constData(), vertices.size() * sizeof (GLfloat) * 3);
    vbo.release();
}

float PerlinNoise2D::altitude(float x, float y)
{
    float z = 0.0;
    float p = 1.0;
    int f = 1;

    for(int j = 0; j < octave; ++j) {
        z += p * noise(x * f, y * f);
        p *= persistance;
        f *= 2;
    }
    z *= (1.0 - persistance) / (1.0 - p);
    return z;
}

float PerlinNoise2D::width() const
{
    return _width;
}

float PerlinNoise2D::height() const
{
    return _height;
}

float PerlinNoise2D::interp1d(float a, float b, float x)
{
    float k = (1.0 - std::cos(x * M_PI)) / 2.0;
    return a * (1.0 - k) + b * k;
}

float PerlinNoise2D::interp2d(float a, float b, float c, float d, float x, float y)
{
    float y1 = interp1d(a, b, x);
    float y2 = interp1d(c, d, x);
    return interp1d(y1, y2, y);
}

float PerlinNoise2D::noise(float x, float y)
{
    x /= step;
    y /= step;
    int i = int(x);
    int j = int(y);
    return interp2d(randoms.value(i * w + j),
                    randoms.value((i+1) * w + j),
                    randoms.value(i * w + (j+1)),
                    randoms.value((i+1) * w + (j+1)),
                    std::fmod(x, 1.0f),
                    std::fmod(y, 1.0f));
}
