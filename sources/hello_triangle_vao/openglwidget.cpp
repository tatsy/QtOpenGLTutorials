#include "openglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

static QVector3D vertices[3] = {
    QVector3D(-0.5f, -0.5f, 0.0f),
    QVector3D(-0.5f,  0.5f, 0.0f),
    QVector3D( 0.5f,  0.5f, 0.0f)
};

static QVector3D colors[3] = {
    QVector3D(1.0f, 0.0f, 0.0f),
    QVector3D(0.0f, 1.0f, 0.0f),
    QVector3D(0.0f, 0.0f, 1.0f)
};

static uint32_t indices[3] = { 0, 1, 2 };

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OpenGLWidget::~OpenGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OpenGLWidget::initializeGL() {
    // Initialize Qt's OpenGL functions.
    auto f = QOpenGLContext::currentContext()->versionFunctions();
    f->initializeOpenGLFunctions();

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo->bind();
    vbo->allocate(sizeof(float) * (3 + 3) * 3);

    uint32_t offset = 0;
    glEnableClientState(GL_VERTEX_ARRAY);
    vbo->write(offset, vertices, sizeof(QVector3D) * 3);
    glVertexPointer(3, GL_FLOAT, 0, (void *)offset);

    offset += sizeof(QVector3D) * 3;
    glEnableClientState(GL_COLOR_ARRAY);
    vbo->write(offset, colors, sizeof(QVector3D) * 3);
    glColorPointer(3, GL_FLOAT, 0, (void *)offset);

    ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    ibo->create();
    ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo->bind();
    ibo->allocate(&indices[0], sizeof(uint32_t) * 3);

    vao->release();
}

void OpenGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw color triangle.
    vao->bind();
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    vao->release();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}