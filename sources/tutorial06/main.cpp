#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>

#include "oglwidget.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    OGLWidget widget;
    widget.resize(800, 600);
    widget.show();

    return app.exec();
}
