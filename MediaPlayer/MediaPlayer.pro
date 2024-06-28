QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(./netapi/netapi.pri)
INCLUDEPATH += ./netapi/

SOURCES += \
    PacketQueue.cpp \
    ckernal.cpp \
    logindialog.cpp \
    main.cpp \
    onlinedialog.cpp \
    playerdialog.cpp \
    qmymovielabel.cpp \
    uploaddialog.cpp \
    videoplayer.cpp

HEADERS += \
    PacketQueue.h \
    ckernal.h \
    logindialog.h \
    onlinedialog.h \
    playerdialog.h \
    qmymovielabel.h \
    uploaddialog.h \
    videoplayer.h

FORMS += \
    logindialog.ui \
    onlinedialog.ui \
    playerdialog.ui \
    qmymovielabel.ui \
    uploaddialog.ui

include(./opengl/opengl.pri)
INCLUDEPATH += ./opengl/

INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include\
                $$PWD/SDL2-2.0.10/include

LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib\
        $$PWD/ffmpeg-4.2.2/lib/avdevice.lib\
        $$PWD/ffmpeg-4.2.2/lib/avfilter.lib\
        $$PWD/ffmpeg-4.2.2/lib/avformat.lib\
        $$PWD/ffmpeg-4.2.2/lib/avutil.lib\
        $$PWD/ffmpeg-4.2.2/lib/postproc.lib\
        $$PWD/ffmpeg-4.2.2/lib/swresample.lib\
        $$PWD/ffmpeg-4.2.2/lib/swscale.lib\
        $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../ClassNeed/images/images/soure.qrc \
    ../ClassNeed/tb/tb/tb.qrc
