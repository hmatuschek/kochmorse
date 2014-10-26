set(kochmorse_SOURCES main.cc
    audiosink.cc effect.cc morseencoder.cc tutor.cc application.cc mainwindow.cc settings.cc
    aboutdialog.cc listwidget.cc)
set(kochmorse_MOC_HEADERS
    audiosink.hh effect.hh morseencoder.hh tutor.hh application.hh mainwindow.hh settings.hh
    aboutdialog.hh listwidget.hh)

qt5_wrap_cpp(kochmorse_MOC_SOURCES ${kochmorse_MOC_HEADERS})

qt5_add_resources(kochmorse_RCC_SOURCES ../resources.qrc)

set(kochmorse_HEADERS ${kochmorse_MOC_HEADERS})
add_executable(kochmorse ${kochmorse_SOURCES} ${kochmorse_MOC_SOURCES} ${kochmorse_RCC_SOURCES})
target_link_libraries(kochmorse ${LIBS})

install(TARGETS kochmorse DESTINATION bin)
