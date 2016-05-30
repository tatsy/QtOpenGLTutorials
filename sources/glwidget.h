#ifdef _MSC_VER
#pragma once
#endif

#ifndef _GLWIDGET_H_
#define _GLWIDGET_H_

#include <memory>
#include <vector>

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglvertexarrayobject.h>
#include <QtGui/qopenglbuffer.h>
#include <QtGui/qopenglframebufferobject.h>

struct Vertex {
    QVector3D pos    = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D normal = QVector3D(0.0f, 0.0f, 0.0f);
    QVector4D color  = QVector4D(1.0f, 1.0f, 1.0f, 0.0f);
    static int posOffset()    { return 0;                     }
    static int normalOffset() { return sizeof(QVector3D);     }
    static int colorOffset()  { return sizeof(QVector3D) * 2; }
};

class GLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void loadMesh(const std::string& filename);

private:
    std::unique_ptr<QOpenGLShaderProgram>     shader = nullptr;
    std::unique_ptr<QOpenGLVertexArrayObject> vao    = nullptr;
    std::unique_ptr<QOpenGLBuffer> vertexBuffer = nullptr;
    std::unique_ptr<QOpenGLBuffer> indexBuffer  = nullptr;
};

#endif  // _GLWIDGET_H_
