#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>

#include "openglwidget.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    // In this tutorial, we use OpenGL 4.1 core for MacOSX support.
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    QSurfaceFormat::setDefaultFormat(format);

    OpenGLWidget widget;
    widget.resize(800, 600);
    widget.setWindowTitle("Blinn-Phong");
    widget.show();

    return app.exec();
}