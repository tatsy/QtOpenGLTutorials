#include "openglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

static constexpr int POSITION_LAYOUT_LOC = 0;
static constexpr int NORMAL_LAYOUT_LOC   = 1;
static constexpr int TEXCOORD_LAYOUT_LOC = 2;

static const QVector3D lightPos = QVector3D(-5.0f, 5.0f, 5.0f);

static double theta = 0.0;

struct Vertex {
    Vertex()
        : position{}
        , normal{}
        , texcoord{} {
    }

    Vertex(const QVector3D &pos, const QVector3D &norm, const QVector2D &uv)
        : position{pos}
        , normal{norm}
        , texcoord{uv} {
    }

    QVector3D position;
    QVector3D normal;
    QVector3D texcoord;
};

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    timer = std::make_unique<QTimer>(this);
    timer->start();
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(onAnimate()));
}

OpenGLWidget::~OpenGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OpenGLWidget::initializeGL() {
    // Initialize Qt's OpenGL functions.
    initializeOpenGLFunctions();
    auto format = QOpenGLContext::currentContext()->format();
    printf("OpenGL: version %d.%d\n", format.majorVersion(), format.minorVersion());

    // General OpenGL settings.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_NONE);

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

    // Load OBJ.
    const std::string filename = std::string(DATA_DIRECTORY) + "teapot.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    
    if (!success) {
        std::exit(1);
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (const auto &s : shapes) {
        for (const auto &index : s.mesh.indices) {
            const int posIndex = index.vertex_index;
            QVector3D position;
            if (posIndex >= 0) {
                position = {
                    attrib.vertices[posIndex * 3 + 0],
                    attrib.vertices[posIndex * 3 + 1],
                    attrib.vertices[posIndex * 3 + 2]
                };
            }

            const int normIndex = index.normal_index;
            QVector3D normal;
            if (normIndex >= 0) {
                normal = {
                    attrib.normals[normIndex * 3 + 0],
                    attrib.normals[normIndex * 3 + 1],
                    attrib.normals[normIndex * 3 + 2]
                };
            }

            const int uvIndex = index.texcoord_index;
            QVector2D texcoord;
            if (uvIndex >= 0) {
                texcoord = {
                    attrib.texcoords[uvIndex * 2 + 0],
                    attrib.texcoords[uvIndex * 2 + 1]
                };
            }

            indices.push_back(vertices.size());
            vertices.emplace_back(position, normal, texcoord);
        }
    }

    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo->bind();
    vbo->allocate(vertices.data(), sizeof(Vertex) * vertices.size());
    
    glEnableVertexAttribArray(POSITION_LAYOUT_LOC);
    glVertexAttribPointer(POSITION_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void *)offsetof(Vertex, position));

    glEnableVertexAttribArray(NORMAL_LAYOUT_LOC);
    glVertexAttribPointer(NORMAL_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(TEXCOORD_LAYOUT_LOC);
    glVertexAttribPointer(TEXCOORD_LAYOUT_LOC, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void *)offsetof(Vertex, texcoord));

    ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    ibo->create();
    ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo->bind();
    ibo->allocate(indices.data(), sizeof(uint32_t) * indices.size());

    vao->release();
}

void OpenGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrices.
    QMatrix4x4 mMat, vMat, pMat;
    mMat.rotate(theta, QVector3D(0.0f, 1.0f, 0.0f));
    vMat.lookAt(QVector3D(3.0f, 4.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    pMat.perspective(45.0f, (float)width() / (float)height(), 0.1f, 1000.0f);

    QMatrix4x4 mvMat = vMat * mMat;
    QMatrix4x4 mvpMat = pMat * mvMat;
    QMatrix4x4 normMat = mvMat.transposed().inverted();

    // Draw color triangle.
    shader->bind();
    vao->bind();

    shader->setUniformValue("u_mvpMat", mvpMat);
    shader->setUniformValue("u_mvMat", mvMat);
    shader->setUniformValue("u_viewMat", vMat);
    shader->setUniformValue("u_normMat", normMat);
    shader->setUniformValue("u_lightPos", lightPos);

    glDrawElements(GL_TRIANGLES, ibo->size() / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

    shader->release();
    vao->release();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}

void OpenGLWidget::onAnimate() {
    theta += 1.0;
    if (theta > 360.0) {
        theta -= 360.0;
    }
    update();
}