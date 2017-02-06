#include "openglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static constexpr int POSITION_LAYOUT_LOC = 0;
static constexpr int NORMAL_LAYOUT_LOC   = 1;
static constexpr int TEXCOORD_LAYOUT_LOC = 2;

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

static const float cubePositions[8][3] = {
    { -128.0f, -128.0f, -128.0f },
    { -128.0f, -128.0f,  128.0f },
    { -128.0f,  128.0f, -128.0f },
    {  128.0f, -128.0f, -128.0f },
    { -128.0f,  128.0f,  128.0f },
    {  128.0f, -128.0f,  128.0f },
    {  128.0f,  128.0f, -128.0f },
    {  128.0f,  128.0f,  128.0f }
};


static const unsigned int cubeIndices[12][3] = {
    { 0, 6, 2 },
    { 0, 3, 6 },
    { 1, 7, 5 },
    { 1, 4, 7 },
    { 0, 4, 1 },
    { 0, 2, 4 },
    { 3, 7, 6 },
    { 3, 5, 7 },
    { 0, 5, 3 },
    { 0, 1, 5 },
    { 2, 7, 4 },
    { 2, 6, 7 }
};

struct Cubemap {
    QImage posX, negX, posY, negY, posZ, negZ;

    void load(const std::string& filename) {
        int w, h, comp;
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &comp, STBI_rgb_alpha);

        const int width  = w / 3;
        const int height = h / 4;
        
        // Positive Z
        {
            const int offsetX = width;
            const int offsetY = 0;
        
            posZ = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = x + offsetX;
                    int yy = y + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    posZ.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }

        // Negative Z
        {
            const int offsetX = width;
            const int offsetY = height * 2;    
            negZ = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = x + offsetX;
                    int yy = y + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    negZ.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }

        // Positive Y
        {
            const int offsetX = width;
            const int offsetY = height;        
            posY = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = x + offsetX;
                    int yy = y + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    posY.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }

        // Negative Y
        {
            const int offsetX = width;
            const int offsetY = height * 3;        
            negY = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = (width - x - 1) + offsetX;
                    int yy = (height - y - 1) + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    negY.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }

        // Positive X
        {
            const int offsetX = width * 2;
            const int offsetY = height;
        
            posX = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = x + offsetX;
                    int yy = y + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    posX.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }

         // Negative X
        {
            const int offsetX = 0;
            const int offsetY = height; 
            negX = QImage(width, height, QImage::Format_ARGB32);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int xx = x + offsetX;
                    int yy = y + offsetY;
                    int r = data[(yy * w + xx) * 4 + 2];
                    int g = data[(yy * w + xx) * 4 + 1];
                    int b = data[(yy * w + xx) * 4 + 0];
                    negX.setPixel(x, y, qRgb(r, g, b));               
                }
            }
        }
    }
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

    // General OpenGL settings.
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    teapot.shader = std::make_unique<QOpenGLShaderProgram>(this);
    teapot.shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SHADER_DIRECTORY) + "render.vert");
    teapot.shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SHADER_DIRECTORY) + "render.frag");
    teapot.shader->link();
    if (!teapot.shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }

    cube.shader = std::make_unique<QOpenGLShaderProgram>(this);
    cube.shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SHADER_DIRECTORY) + "cubemap.vert");
    cube.shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SHADER_DIRECTORY) + "cubemap.frag");
    cube.shader->link();
    if (!cube.shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }

    // Load OBJ.
    std::string filename = std::string(DATA_DIRECTORY) + "teapot.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
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
    teapot.vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    teapot.vao->create();
    teapot.vao->bind();
    
    teapot.vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    teapot.vbo->create();
    teapot.vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    teapot.vbo->bind();
    teapot.vbo->allocate(vertices.data(), sizeof(Vertex) * vertices.size());
    
    glEnableVertexAttribArray(POSITION_LAYOUT_LOC);
    glVertexAttribPointer(POSITION_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, position));
    
    glEnableVertexAttribArray(NORMAL_LAYOUT_LOC);
    glVertexAttribPointer(NORMAL_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, normal));
    
    glEnableVertexAttribArray(TEXCOORD_LAYOUT_LOC);
    glVertexAttribPointer(TEXCOORD_LAYOUT_LOC, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, texcoord));
    
    teapot.ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    teapot.ibo->create();
    teapot.ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    teapot.ibo->bind();
    teapot.ibo->allocate(indices.data(), sizeof(uint32_t) * indices.size());
    
    teapot.vao->release();

    // Prepare cube VAO.
    cube.vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    cube.vao->create();
    cube.vao->bind();

    cube.vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    cube.vbo->create();
    cube.vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    cube.vbo->bind();
    cube.vbo->allocate(&cubePositions[0][0], sizeof(float) * 8 * 3);

    glEnableVertexAttribArray(POSITION_LAYOUT_LOC);
    glVertexAttribPointer(POSITION_LAYOUT_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    cube.ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    cube.ibo->create();
    cube.ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    cube.ibo->bind();
    cube.ibo->allocate(&cubeIndices[0][0], sizeof(unsigned int) * 3 * 12);

    cube.vao->release();

    // Prepare cubemap
    Cubemap cubeImage;
    cubeImage.load(std::string(DATA_DIRECTORY) + "grace_cross.hdr");
    cubemap = std::make_unique<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
    cubemap->create();
    cubemap->setSize(cubeImage.posX.width(), cubeImage.posX.height(), cubeImage.posX.depth());
    cubemap->setFormat(QOpenGLTexture::RGBA8_UNorm);
    cubemap->allocateStorage();
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapPositiveX, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.posX.constBits(), 0);
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapNegativeX, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.negX.constBits(), 0);
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapPositiveY, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.posZ.constBits(), 0);
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapNegativeY, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.negZ.constBits(), 0);
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapPositiveZ, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.posY.constBits(), 0);
    cubemap->setData(0, 0, QOpenGLTexture::CubeMapFace::CubeMapNegativeZ, QOpenGLTexture::PixelFormat::RGBA,
        QOpenGLTexture::PixelType::UInt8, (const void*)cubeImage.negY.constBits(), 0);

    cubemap->setAutoMipMapGenerationEnabled(false);
    cubemap->setWrapMode(QOpenGLTexture::CoordinateDirection::DirectionS, QOpenGLTexture::WrapMode::ClampToEdge);
    cubemap->setWrapMode(QOpenGLTexture::CoordinateDirection::DirectionT, QOpenGLTexture::WrapMode::ClampToEdge);
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

    // Draw cubemap.
    cube.shader->bind();
    cube.vao->bind();

    cubemap->bind(0);    
    cube.shader->setUniformValue("u_cubemap", 0);
    cube.shader->setUniformValue("u_mvpMat", pMat * vMat);

    glDrawElements(GL_TRIANGLES, cube.ibo->size() / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

    cube.shader->release();
    cube.vao->release();

    // Draw color triangle.
    teapot.shader->bind();
    teapot.vao->bind();

    cubemap->bind(0);
    teapot.shader->setUniformValue("u_cubemap", 0);
    teapot.shader->setUniformValue("u_mvpMat", mvpMat);
    teapot.shader->setUniformValue("u_mvMat", mvMat);
    teapot.shader->setUniformValue("u_normMat", normMat);
    
    glDrawElements(GL_TRIANGLES, teapot.ibo->size() / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

    teapot.shader->release();
    teapot.vao->release();
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
