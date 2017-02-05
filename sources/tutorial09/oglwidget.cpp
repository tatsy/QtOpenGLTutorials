#include "oglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

static constexpr int SHADER_POSITION_LOC = 0;
static constexpr int SHADER_COLOR_LOC    = 1;

static QVector3D vertices[3] = {
    QVector3D(-0.5f, -0.5f, 0.0f),
    QVector3D(-0.5f,  0.5f, 0.0f),
    QVector3D( 0.5f,  0.5f, 0.0f)
};

static QVector4D colors[3] = {
    QVector4D(1.0f, 0.0f, 0.0f, 1.0f),
    QVector4D(0.0f, 1.0f, 0.0f, 1.0f),
    QVector4D(0.0f, 0.0f, 1.0f, 1.0f)
};

static unsigned int indices[3] = { 0, 1, 2 };

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OGLWidget::~OGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OGLWidget::initializeGL() {
    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SOURCE_DIRECTORY) + "render.vs");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SOURCE_DIRECTORY) + "render.fs");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }

    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vBuffer->create();
    vBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vBuffer->bind();
    vBuffer->allocate(sizeof(float) * (3 + 4) * 3);
    vBuffer->write(0, &vertices[0], sizeof(QVector3D) * 3);
    vBuffer->write(sizeof(QVector3D) * 3, &colors[0], sizeof(QVector4D) * 3);

    auto f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(SHADER_POSITION_LOC);
    f->glEnableVertexAttribArray(SHADER_COLOR_LOC);
    f->glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);
    f->glVertexAttribPointer(SHADER_COLOR_LOC,    4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(QVector3D) * 3));

    iBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    iBuffer->create();
    iBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    iBuffer->bind();
    iBuffer->allocate(&indices[0], sizeof(unsigned int) * 3);

    vao->release();
}

void OGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw color triangle.
    shader->bind();
    vao->bind();
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    shader->release();
    vao->release();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}