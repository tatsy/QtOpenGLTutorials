#ifndef _OGL_WIDGET_H_
#define _OGL_WIDGET_H_

#include <QtWidgets/qopenglwidget.h>

class OGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    OGLWidget(QWidget* parent = nullptr);
    ~OGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};

#endif  // _OGL_WIDGET_H_
