set(kochmorse_SOURCES main.cc
    globals.cc audiosink.cc effect.cc morseencoder.cc tutor.cc application.cc mainwindow.cc
    settings.cc aboutdialog.cc listwidget.cc morsedecoder.cc)
set(kochmorse_MOC_HEADERS
    audiosink.hh effect.hh morseencoder.hh tutor.hh application.hh mainwindow.hh settings.hh
    aboutdialog.hh listwidget.hh morsedecoder.hh)
set(kochmorse_HEADERS globals.hh ${kochmorse_MOC_HEADERS})

qt5_wrap_cpp(kochmorse_MOC_SOURCES ${kochmorse_MOC_HEADERS})
qt5_add_resources(kochmorse_RCC_SOURCES ../resources.qrc)

add_executable(kochmorse ${kochmorse_SOURCES} ${kochmorse_MOC_SOURCES} ${kochmorse_RCC_SOURCES})
target_link_libraries(kochmorse ${LIBS})

install(TARGETS kochmorse DESTINATION bin)
