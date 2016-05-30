#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>

#include "glwidget.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    QSurfaceFormat::setDefaultFormat(format);

    GLWidget win;
    win.resize(800, 600);
    win.show();

    return app.exec();
}