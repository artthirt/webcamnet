TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT += gui widgets opengl

DEFINES += _JPEG

LIBJPEG_DIR = $$PWD/3rd/libjpeg
OPENJPEG_DIR = $$PWD/3rd/openjpeg
OPENCV_DIR = d:/devs/opencv/build
BOOST_DIR = d:\\devs\\Boost\\

INCLUDEPATH += utils \
                $$LIBJPEG_DIR/include \
                $$OPENJPEG_DIR/include \
                $$OPENCV_DIR/include \
                $$BOOST_DIR\\include\\boost-1_73\\

LIBS += -L$$LIBJPEG_DIR/lib -L$$OPENJPEG_DIR/bin -L$$OPENCV_DIR/x64/vc15/bin -L$$BOOST_DIR/lib

SOURCES += main.cpp \
    listen_asf.cpp \
    mainwindow.cpp \
    outputimage.cpp \
	utils/asf_stream.cpp \
	utils/datastream.cpp \
        openjp_decode.cpp \
        jpeg_decode.cpp \
	utils/common.cpp

HEADERS += \
    listen_asf.h \
    mainwindow.h \
    outputimage.h \
    structs.h \
	utils/asf_stream.h \
	utils/common.h \
	utils/datastream.h \
        openjp_decode.h \
        jpeg_decode.h

win32{
    CONFIG(debug,   debug|release): LIBS += -lopencv_world451d
    CONFIG(release, debug|release): LIBS += -lopencv_world451

    LIBS += -lopenjp2 -ljpeg-static
}else{
    LIBS += -lopenjp2 -lopencv_core -lopencv_highgui \
                     -lboost_system -lboost_thread -ljpeg
}

include(asio.pri)

FORMS += \
    mainwindow.ui

RESOURCES += \
    res.qrc
