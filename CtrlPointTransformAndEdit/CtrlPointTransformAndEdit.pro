include(GraphicsCommon.pri)

QT += widgets

DESTDIR=bin #Target file directory
OBJECTS_DIR=generated_files #Intermediate object files directory
MOC_DIR=generated_files #Intermediate moc files directory

SOURCES += \
    main.cpp \
    TransformEditView.cpp \
    TransformEditController.cpp \
    OpenGLWidget.cpp

QMAKE_MAC_SDK = macosx10.12

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

HEADERS += \
    TransformEditView.h \
    TransformEditController.h \
    OpenGLWidget.h

FORMS += \
    MyControllerWidget.ui
