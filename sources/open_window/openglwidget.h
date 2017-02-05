#ifndef _OGL_WIDGET_H_
#define _OGL_WIDGET_H_

#include <QtWidgets/qopenglwidget.h>

class OpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};

#endif  // _OGL_WIDGET_H_
