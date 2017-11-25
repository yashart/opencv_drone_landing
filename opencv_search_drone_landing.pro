TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3


SOURCES += main.cpp \
    generate_resized_images.cpp \
    surf.cpp \
    red_detection.cpp \
    hsv_color.cpp

unix
{
    INCLUDEPATH += /usr/include/opencv2
    LIBS += -L/usr/lib
    LIBS += -lopencv_core
    LIBS += -lopencv_imgproc
    LIBS += -lopencv_highgui
    LIBS += -lopencv_imgcodecs
    LIBS += -lopencv_features2d
    LIBS += -lopencv_calib3d
    LIBS += -lopencv_videoio
}
