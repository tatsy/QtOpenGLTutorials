#include "openglwidget.h"

#include <cmath>
#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "common.h"

static constexpr int SHADER_POSITION_LOC = 0;
static constexpr int SHADER_NORMAL_LOC   = 1;
static constexpr int SHADER_TEXCOORD_LOC = 2;

static const QVector3D LIGHT_POS = QVector3D(-5.0f, 5.0f, 5.0f);

static const float PI = 4.0f * std::atan(1.0f);
static float theta = 0.0f;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(onAnimate()));
    timer->start();
}

OpenGLWidget::~OpenGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OpenGLWidget::initializeGL() {
    // Initialize OpenGL functions
    auto f = QOpenGLContext::currentContext()->versionFunctions();
    f->initializeOpenGLFunctions();

    // General OpenGL settings.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SHADER_DIRECTORY) + "render.vert");
    shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, QString(SHADER_DIRECTORY) + "render.tesc");
    shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, QString(SHADER_DIRECTORY) + "render.tese");
    shader->addShaderFromSourceFile(QOpenGLShader::Geometry, QString(SHADER_DIRECTORY) + "render.geom");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SHADER_DIRECTORY) + "render.frag");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }
    
    // Load OBJ.
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    const std::string objfile = std::string(DATA_DIRECTORY) + "plane.obj";
    bool success = tinyobj::LoadObj(shapes, materials, error, objfile.c_str());
    if (!error.empty()) {
        std::cerr << error << std::endl;
    }
    
    if (!success) {
        std::cerr << "Failed to load OBJ file: " << objfile << std::endl;
        std::exit(1);
    }
    
    auto mesh = shapes[0].mesh;

    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();
    
    vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo->bind();
    
    const uint32_t totalSize = (mesh.positions.size() + mesh.normals.size() + mesh.texcoords.size());
    vbo->allocate(sizeof(float) * totalSize);

    int offset = 0;
    vbo->write(offset, mesh.positions.data(), sizeof(float) * mesh.positions.size());
    glEnableVertexAttribArray(SHADER_POSITION_LOC);
    glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);

    offset += sizeof(float) * mesh.positions.size();
    vbo->write(offset, mesh.normals.data(), sizeof(float) * mesh.normals.size());
    glEnableVertexAttribArray(SHADER_NORMAL_LOC);
    glVertexAttribPointer(SHADER_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);

    offset += sizeof(float) * mesh.normals.size();
    vbo->write(offset, mesh.texcoords.data(), sizeof(float) * mesh.texcoords.size());
    glEnableVertexAttribArray(SHADER_TEXCOORD_LOC);
    glVertexAttribPointer(SHADER_TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    
    ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    ibo->create();
    ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo->bind();
    ibo->allocate(mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());

    vao->release();
    
    // Prepare texture.
    QImage texImage;
    texImage.load(QString(DATA_DIRECTORY) + "blocks.jpg");
    texture = std::make_unique<QOpenGLTexture>(texImage, QOpenGLTexture::GenerateMipMaps);
    
    // Setup default tesselation control parameters.
    const float innerTessLevels[] = { 8.0f, 8.0f };
    const float outerTessLevels[] = { 2.0f, 4.0f, 6.0f, 8.0f };
    glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerTessLevels);
    glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerTessLevels);
}

void OpenGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrices.
    QMatrix4x4 projMat, viewMat, modelMat;
    projMat.perspective(45.0f, (float)width() / (float)height(), 0.1f, 1000.0f);
    viewMat.lookAt(QVector3D(3.0f, 4.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    modelMat.rotate(theta, QVector3D(0.0f, 1.0f, 0.0f));

    // Draw color triangle.
    shader->bind();
    vao->bind();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureId());

    shader->setUniformValue("u_projMat", projMat);
    shader->setUniformValue("u_viewMat", viewMat);
    shader->setUniformValue("u_modelMat", modelMat);
    shader->setUniformValue("u_lightPos", LIGHT_POS);
    shader->setUniformValue("u_texture", 0);
    
    glDrawElements(GL_PATCHES, ibo->size() / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader->release();
    vao->release();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}

void OpenGLWidget::onAnimate() {
    theta += 0.5;
    if (theta > 360.0) {
        theta -= 360.0;
    }
    this->update();
}
