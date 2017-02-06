#include <QtWidgets/qapplication.h>

#include "openglwidget.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    OpenGLWidget widget;
    widget.resize(800, 600);
    widget.setWindowTitle("Hello triangle (VAO)");
    widget.show();

    return app.exec();
}