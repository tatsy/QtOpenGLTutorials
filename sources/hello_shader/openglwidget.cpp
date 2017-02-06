#include "openglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

static constexpr int POSITION_LAYOUT_LOC = 0;
static constexpr int COLOR_LAYOUT_LOC = 1;

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
    initializeOpenGLFunctions();
    auto format = QOpenGLContext::currentContext()->format();
    printf("OpenGL: version %d.%d\n", format.majorVersion(), format.minorVersion());

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, 
        QString(SHADER_DIRECTORY) + "render.vert");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
        QString(SHADER_DIRECTORY) + "render.frag");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to build shaders!" << std::endl;
        std::exit(1);
    }

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
    vbo->write(offset, vertices, sizeof(QVector3D) * 3);
    glEnableVertexAttribArray(POSITION_LAYOUT_LOC);
    glVertexAttribPointer(POSITION_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void *)offset);                               

    offset += sizeof(QVector3D) * 3;
    vbo->write(offset, colors, sizeof(QVector3D) * 3);
    glEnableVertexAttribArray(COLOR_LAYOUT_LOC);
    glVertexAttribPointer(COLOR_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void *)offset);

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
    shader->bind();
    vao->bind();
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    shader->release();
    vao->release();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}