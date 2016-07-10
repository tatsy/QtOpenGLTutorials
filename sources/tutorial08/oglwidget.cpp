#include "oglwidget.h"

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

static constexpr int SHADER_POSITION_LOC = 0;
static constexpr int SHADER_NORMAL_LOC   = 1;
static constexpr int SHADER_TEXCOORD_LOC = 2;

static constexpr int CUBE_POSITION_LOC = 0;

static const QVector3D LIGHT_POS = QVector3D(-5.0f, 5.0f, 5.0f);

static const float cubeVertices[8][3] = {
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

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    controller = std::make_unique<ArcballController>(this);

    QMatrix4x4 mMat, vMat;
    mMat.setToIdentity();
    vMat.lookAt(QVector3D(4.0f, 4.0f, 4.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    controller->initModelView(mMat, vMat);
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

    cubeShader = std::make_unique<QOpenGLShaderProgram>(this);
    cubeShader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SOURCE_DIRECTORY) + "cubemap.vs");
    cubeShader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SOURCE_DIRECTORY) + "cubemap.fs");
    cubeShader->link();
    if (!cubeShader->isLinked()) {
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

    // Prepare cube VAO.
    cubeVao = std::make_unique<QOpenGLVertexArrayObject>(this);
    cubeVao->create();
    cubeVao->bind();

    cubeVBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    cubeVBuffer->create();
    cubeVBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    cubeVBuffer->bind();
    cubeVBuffer->allocate(&cubeVertices[0][0], sizeof(float) * 8 * 3);

    f->glEnableVertexAttribArray(CUBE_POSITION_LOC);
    f->glVertexAttribPointer(CUBE_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    cubeIBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    cubeIBuffer->create();
    cubeIBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    cubeIBuffer->bind();
    cubeIBuffer->allocate(&cubeIndices[0][0], sizeof(unsigned int) * 3 * 12);

    cubeVao->release();

    // Prepare cubemap
    Cubemap cubeImage;
    cubeImage.load(std::string(SOURCE_DIRECTORY) + "grace_cross.hdr");
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

void OGLWidget::paintGL() {
    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrices.
    QMatrix4x4 pMat;
    pMat.perspective(45.0f, (float)width() / (float)height(), 0.1f, 1000.0f);
    QMatrix4x4 mvMat  = controller->modelViewMat();
    QMatrix4x4 mvpMat = pMat * mvMat;

    // Draw cubemap.
    cubeShader->bind();
    cubeVao->bind();

    cubemap->bind(0);    
    cubeShader->setUniformValue("uCubemap", 0);
    cubeShader->setUniformValue("uMVPMat", mvpMat);

    glDrawElements(GL_TRIANGLES, cubeIBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    cubeShader->release();
    cubeVao->release();

    // Draw color triangle.
    shader->bind();
    vao->bind();

    cubemap->bind(0);
    cubeShader->setUniformValue("uCubemap", 0);
    shader->setUniformValue("uMVPMat", mvpMat);
    shader->setUniformValue("uMVMat", mvMat);
    
    glDrawElements(GL_TRIANGLES, iBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    shader->release();
    vao->release();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}

void OGLWidget::mousePressEvent(QMouseEvent *ev) {
    controller->setOldPoint(ev->pos());
    controller->setNewPoint(ev->pos());
    if (ev->button() == Qt::LeftButton) {
        controller->setMode(ArcballMode::Rotate);
    } else if (ev->button() == Qt::RightButton) {
        controller->setMode(ArcballMode::Translate);
    } else if (ev->button() == Qt::MiddleButton) {
        controller->setMode(ArcballMode::Scale);
    }
}

void OGLWidget::mouseMoveEvent(QMouseEvent *ev) {
    controller->setNewPoint(ev->pos());
    controller->update();
    controller->setOldPoint(ev->pos());
    update();
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *ev) {
    controller->setMode(ArcballMode::None);
}

void OGLWidget::wheelEvent(QWheelEvent *ev) {
    controller->setScroll(controller->scroll() + ev->delta() / 1000.0);
    controller->update();
    update();
}
