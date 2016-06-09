#include "oglwidget.h"

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OGLWidget::~OGLWidget() {
}

void OGLWidget::initializeGL() {
    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OGLWidget::paintGL() {
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

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}