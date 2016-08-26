TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += _JPEG

INCLUDEPATH += utils

SOURCES += main.cpp \
	utils/asf_stream.cpp \
	utils/datastream.cpp \
    openjp_decode.cpp \
    jpeg_decode.cpp \
	utils/common.cpp

HEADERS += \
	utils/asf_stream.h \
	utils/common.h \
	utils/datastream.h \
    openjp_decode.h \
    jpeg_decode.h

LIBS += -lopenjp2 -lopencv_core -lopencv_highgui \
		 -lboost_system -lboost_thread -ljpeg

