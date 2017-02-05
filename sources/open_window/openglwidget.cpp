#include "openglwidget.h"
#include "common.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OpenGLWidget::~OpenGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OpenGLWidget::initializeGL() {
    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OpenGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}
