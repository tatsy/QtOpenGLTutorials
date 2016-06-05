#include "glwidget.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "common.h"

static const int SHADER_POSITION_LOCATION = 0;
static const int SHADER_NORMAL_LOCATION   = 1;
static const int SHADER_COLOR_LOCATION    = 2;

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget{ parent } {
}

GLWidget::~GLWidget() {
}

void GLWidget::initializeGL() {
    // Default OpenGL setting.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize shaders
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex,   QString(SOURCE_DIRECTORY) + "render.vs");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SOURCE_DIRECTORY) + "render.fs");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to link shaders!!" << std::endl;
        std::exit(1);
    }

    // Load data
    loadMesh(std::string(DATA_DIRECTORY) + "teapot.obj");
}

void GLWidget::paintGL() {
    // Enable shader.
    shader->bind();

    // Bind buffers.
    vao->bind();

    // Clear default render buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrices setup
    QMatrix4x4 mMat, vMat, pMat;
    mMat.setToIdentity();
    vMat.lookAt(QVector3D(6.0f, 8.0f, 10.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    pMat.perspective(45.0f, (float)width() / height(), 1.0f, 1000.0f);
    
    QMatrix4x4 mvpMat = pMat * vMat * mMat;
    QMatrix4x4 mvMat  = vMat * mMat;
    shader->setUniformValue("uMVPMat", mvpMat);
    shader->setUniformValue("uMVMat", mvMat);
    shader->setUniformValue("uLightPos", QVector3D(10.0f, 20.0f, 30.0f));

    // Draw
    glDrawElements(GL_TRIANGLES, indexBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    // Unbind buffers.
    vao->release();

    // Disable shader.
    shader->release();
}

void GLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, this->width(), this->height());
}

void GLWidget::loadMesh(const std::string& filename) {
    std::ifstream ifs(filename.c_str(), std::ios::in);
    if (!ifs.is_open()) {
        fprintf(stderr, "FILE: %s not found!!", filename.c_str());
        std::exit(1);
    }

    std::vector<QVector3D> positions;
    std::vector<unsigned int> indices;

    std::string line;
    while (!ifs.eof()) {
        std::getline(ifs, line);    
        if (line[0] == '#') continue;
        
        if (line[0] == 'v') {
            std::stringstream ss;
            ss << line;
            char v;
            float x, y, z;
            ss >> v >> x >> y >> z;
            positions.emplace_back(x, y, z);
        } else if (line[0] == 'f') {
            std::stringstream ss;
            ss << line;
            char f;
            int a, b, c;
            ss >> f >> a >> b >> c;
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }
    ifs.close();

    std::vector<QVector3D> normals(positions.size(), QVector3D(0.0f, 0.0f, 0.0f));
    std::vector<int> degree(positions.size(), 0);
    for (int i = 0; i < indices.size(); i += 3) {
        QVector3D v0 = positions[indices[i]];
        QVector3D v1 = positions[indices[i + 1]];
        QVector3D v2 = positions[indices[i + 2]];
        QVector3D nrm = QVector3D::crossProduct(v1 - v0, v2 - v0).normalized();
        normals[indices[i]] += nrm;
        normals[indices[i + 1]] += nrm;
        normals[indices[i + 2]] += nrm;
        degree[indices[i]] += 1;
        degree[indices[i + 1]] += 1;
        degree[indices[i + 2]] += 1;
    }

    std::vector<Vertex> vertices(normals.size()); 
    for (int i = 0; i < normals.size(); i++) {
        if (degree[i] != 0) {
            normals[i] /= degree[i];
        }
        vertices[i].pos    = positions[i];
        vertices[i].normal = normals[i];
        vertices[i].color  = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    printf("Vertices: %zu\n", vertices.size());
    printf("   Faces: %zu\n", indices.size());

    // Initialize VBO
    shader->bind();

    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vertexBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vertexBuffer->create();
    vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexBuffer->bind();
    vertexBuffer->allocate(&vertices[0], vertices.size() * sizeof(Vertex));

    shader->enableAttributeArray(SHADER_POSITION_LOCATION);
    shader->enableAttributeArray(SHADER_NORMAL_LOCATION);
    shader->enableAttributeArray(SHADER_COLOR_LOCATION);

    shader->setAttributeBuffer(SHADER_POSITION_LOCATION, GL_FLOAT, Vertex::posOffset(),    3, sizeof(Vertex));
    shader->setAttributeBuffer(SHADER_NORMAL_LOCATION,   GL_FLOAT, Vertex::normalOffset(), 3, sizeof(Vertex));
    shader->setAttributeBuffer(SHADER_COLOR_LOCATION,    GL_FLOAT, Vertex::colorOffset(),  4, sizeof(Vertex));

    indexBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    indexBuffer->create();
    indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer->bind();
    indexBuffer->allocate(&indices[0], indices.size() * sizeof(unsigned int));

    vao->release();
    shader->release();
}