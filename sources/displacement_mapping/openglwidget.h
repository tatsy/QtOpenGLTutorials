#ifndef _OGL_WIDGET_H_
#define _OGL_WIDGET_H_

#include <memory>

#include <QtCore/qtimer.h>
#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglvertexarrayobject.h>
#include <QtGui/qopenglbuffer.h>
#include <QtGui/qopengltexture.h>
#include <QtGui/qevent.h>

class OpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    
private slots:
    void onAnimate();
    
private:
    std::unique_ptr<QTimer> timer = nullptr;
    std::unique_ptr<QOpenGLShaderProgram> shader = nullptr;
    std::unique_ptr<QOpenGLVertexArrayObject> vao = nullptr;
    std::unique_ptr<QOpenGLBuffer> vbo = nullptr;
    std::unique_ptr<QOpenGLBuffer> ibo = nullptr;
    std::unique_ptr<QOpenGLTexture> texture = nullptr;
};

#endif  // _OGL_WIDGET_H_
