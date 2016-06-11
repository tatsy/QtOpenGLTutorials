#include "oglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

static constexpr int SHADER_POSITION_LOC = 0;
static constexpr int SHADER_NORMAL_LOC   = 1;
static constexpr int SHADER_TEXCOORD_LOC = 2;

static const QVector3D LIGHT_POS = QVector3D(-5.0f, 5.0f, 5.0f);

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
}

OGLWidget::~OGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OGLWidget::initializeGL() {
    // General OpenGL settings.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SOURCE_DIRECTORY) + "render.vs");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SOURCE_DIRECTORY) + "render.fs");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }

    // Load OBJ.
    std::string filename = std::string(SOURCE_DIRECTORY) + "teapot.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(shapes, materials, err, filename.c_str(),
        SOURCE_DIRECTORY, tinyobj::calculate_normals | tinyobj::triangulation);
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    
    if (!success) {
        std::exit(1);
    }

    tinyobj::mesh_t& mesh = shapes[0].mesh;

    shader->bind();
    tinyobj::material_t& m = materials[0];
    shader->setUniformValue("uDiffuse",   m.diffuse[0],  m.diffuse[1],  m.diffuse[2]);
    shader->setUniformValue("uSpecular",  m.specular[0], m.specular[1], m.specular[2]);
    shader->setUniformValue("uAmbient",   m.ambient[0],  m.ambient[1],  m.ambient[2]);
    shader->setUniformValue("uShininess", m.shininess);
    shader->release();

    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vBuffer->create();
    vBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vBuffer->bind();
    vBuffer->allocate(sizeof(float) * (mesh.positions.size() + 
                                       mesh.normals.size() +
                                       mesh.texcoords.size()));
    
    int offset = 0;
    vBuffer->write(offset, &mesh.positions[0], sizeof(float) * mesh.positions.size());
    offset += sizeof(float) * mesh.positions.size();
    vBuffer->write(offset, &mesh.normals[0],   sizeof(float) * mesh.normals.size());
    offset += sizeof(float) * mesh.normals.size();
    vBuffer->write(offset, &mesh.texcoords[0], sizeof(float) * mesh.texcoords.size());

    auto f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(SHADER_POSITION_LOC);
    f->glEnableVertexAttribArray(SHADER_NORMAL_LOC);
    f->glEnableVertexAttribArray(SHADER_TEXCOORD_LOC);

    offset = 0;
    f->glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    offset += sizeof(float) * mesh.positions.size();
    f->glVertexAttribPointer(SHADER_NORMAL_LOC,   3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    offset += sizeof(float) * mesh.normals.size();
    f->glVertexAttribPointer(SHADER_TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);

    iBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    iBuffer->create();
    iBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    iBuffer->bind();
    iBuffer->allocate(&mesh.indices[0], mesh.indices.size() * sizeof(unsigned int));

    vao->release();
}

void OGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrices.
    QMatrix4x4 mMat, vMat, pMat;
    mMat.setToIdentity();
    vMat.lookAt(QVector3D(4.0f, 5.0f, 6.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    pMat.perspective(45.0f, (float)width() / (float)height(), 0.1f, 1000.0f);
    QMatrix4x4 mvMat  = vMat * mMat;
    QMatrix4x4 mvpMat = pMat * mvMat;

    // Draw color triangle.
    shader->bind();
    vao->bind();

    shader->setUniformValue("uMVPMat", mvpMat);
    shader->setUniformValue("uMVMat", mvMat);
    shader->setUniformValue("uLightPos", LIGHT_POS);

    glDrawElements(GL_TRIANGLES, iBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    shader->release();
    vao->release();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}