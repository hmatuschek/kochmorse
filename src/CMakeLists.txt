set(kochmorse_SOURCES main.cc
    audiosink.cc morseencoder.cc trainer.cc application.cc mainwindow.cc)
set(kochmorse_MOC_HEADERS
    audiosink.hh morseencoder.hh trainer.hh application.hh mainwindow.hh)
qt5_wrap_cpp(kochmorse_MOC_SOURCES ${kochmorse_MOC_HEADERS})

set(kochmorse_HEADERS ${kochmorse_MOC_HEADERS})
add_executable(kochmorse ${kochmorse_SOURCES} ${kochmorse_MOC_SOURCES})
target_link_libraries(kochmorse ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${LIBS})
