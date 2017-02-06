#ifndef _OGL_WIDGET_H_
#define _OGL_WIDGET_H_

#include <memory>

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglvertexarrayobject.h>
#include <QtGui/qopenglbuffer.h>

class OpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    std::unique_ptr<QOpenGLVertexArrayObject> vao = nullptr;
    std::unique_ptr<QOpenGLBuffer> vbo = nullptr;
    std::unique_ptr<QOpenGLBuffer> ibo = nullptr;
};

#endif  // _OGL_WIDGET_H_
