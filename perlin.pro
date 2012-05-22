#-------------------------------------------------
#
# Project created by QtCreator 2012-05-20T21:39:33
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = perlin
TEMPLATE = app


SOURCES += main.cpp\
        glwidget.cpp \
    perlinnoise2d.cpp

HEADERS  += glwidget.h \
    perlinnoise2d.h

OTHER_FILES += \
    perlinnoise2d.vert \
    perlinnoise2d.frag

RESOURCES += \
    perlinnoise2d.qrc
