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

set(decoder_test_SOURCES kochmorse_decoder.cc globals.cc audiosink.cc morseencoder.cc morsedecoder.cc
    decoderwindow.cc)
set(decoder_test_MOC_HEADERS audiosink.hh morseencoder.hh morsedecoder.hh
    decoderwindow.hh)
qt5_wrap_cpp(decoder_test_MOC_SOURCES ${decoder_test_MOC_HEADERS})
add_executable(decoder_test ${decoder_test_SOURCES} ${decoder_test_MOC_SOURCES})
target_link_libraries(decoder_test ${LIBS})

install(TARGETS kochmorse DESTINATION bin)
