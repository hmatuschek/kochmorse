CONFIG += qt debug

QT += gui widgets

SOURCES += src/aboutdialog.cc src/application.cc src/main.cc \
           src/audiosink.cc src/mainwindow.cc src/morseencoder.cc \
           src/settings.cc src/tutor.cc 
HEADERS += src/aboutdialog.hh src/application.hh src/audiosink.hh \
           src/mainwindow.hh src/morseencoder.hh src/settings.hh \
           src/tutor.hh

RESOURCES += resources.qrc

LIBS += -L/opt/local/lib -lportaudio

INCLUDEPATH += /opt/local/include

TARGET = kochmorse
