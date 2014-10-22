set(kochmorse_SOURCES main.cc
    audiosink.cc morseencoder.cc)
set(kochmorse_MOC_HEADERS
    audiosink.hh morseencoder.hh)
qt5_wrap_cpp(kochmorse_MOC_SOURCES ${kochmorse_MOC_HEADERS})

set(kochmorse_HEADERS ${kochmorse_MOC_HEADERS})
add_executable(kochmorse ${kochmorse_SOURCES} ${kochmorse_MOC_SOURCES})
target_link_libraries(kochmorse ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${LIBS})
