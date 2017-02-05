#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>

#include "openglwidget.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    QSurfaceFormat::setDefaultFormat(format);

    OpenGLWidget widget;
    widget.resize(800, 600);
    widget.setWindowTitle("Displacement mapping");
    widget.show();

    return app.exec();
}
