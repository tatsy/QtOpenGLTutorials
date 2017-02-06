#include "openglwidget.h"

#include <QtGui/qopenglcontext.h>

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
    // Initialize Qt's OpenGL functions.
    auto f = QOpenGLContext::currentContext()->versionFunctions();
    f->initializeOpenGLFunctions();

    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw color triangle.
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 1.0f);
    glEnd();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}