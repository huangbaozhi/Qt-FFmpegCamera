QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    playimage.cpp \
    playimagewidget.cpp \
    widget.cpp

HEADERS += \
    playimage.h \
    playimagewidget.h \
    widget.h

FORMS += \
    widget.ui

# ffmpeg读取视频图像模块
include(./VideoPlay/VideoPlay.pri)
INCLUDEPATH += ./VideoPlay

#  定义程序版本号
VERSION = 1.3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
TARGET  = ffmpegCamera

#INCLUDEPATH += E:\A_Work\Code\QtProject\ffmpegVideo\ffmpegCamera\ffmpeg\include
##LIBS += -L E:\A_Work\Code\QtProject\ffmpegVideo\ffmpegCamera\ffmpeg\lib\*.lib
#LIBS += -L E:\A_Work\Code\QtProject\ffmpegVideo\ffmpegCamera\ffmpeg\lib -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

INCLUDEPATH += E:\A_hbzWork\code\Vessel_Code\LbkProjectVessel-build\ffmpeg\include
LIBS += -L E:\A_hbzWork\code\Vessel_Code\LbkProjectVessel-build\ffmpeg\lib -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice
