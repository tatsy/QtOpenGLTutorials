#include "oglwidget.h"
#include "common.h"

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OGLWidget::~OGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OGLWidget::initializeGL() {
    // Set color to clear window (yellow).
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
}

void OGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT);
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}