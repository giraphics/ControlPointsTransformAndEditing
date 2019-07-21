#include <QApplication>
#include "OpenGLWidget.h"
#include "TransformEditView.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    ControllerWidgetImp widget(NULL);
    widget.resize(1400, 600);
    widget.show();

    return app.exec();
}
