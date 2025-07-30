set(kochmorse_SOURCES main.cc qhal.cc textcompare.cc
    globals.cc audiosink.cc effect.cc morseencoder.cc tutor.cc application.cc mainwindow.cc
    settings.cc aboutdialog.cc listwidget.cc morsedecoder.cc textgen.cc qsochat.cc qrm.cc
  checkupdate.cc colorbutton.cc highscore.cc logger.cc)
set(kochmorse_MOC_HEADERS
    audiosink.hh effect.hh morseencoder.hh tutor.hh application.hh mainwindow.hh settings.hh
    aboutdialog.hh listwidget.hh morsedecoder.hh textgen.hh qsochat.hh qrm.hh
  checkupdate.hh colorbutton.hh highscore.hh logger.hh)
set(kochmorse_HEADERS globals.hh qhal.hh textcompare.hh ${kochmorse_MOC_HEADERS})

qt6_wrap_cpp(kochmorse_MOC_SOURCES ${kochmorse_MOC_HEADERS})
qt6_add_resources(kochmorse_RCC_SOURCES ../resources.qrc)

add_executable(kochmorse WIN32 ${kochmorse_SOURCES} ${kochmorse_MOC_SOURCES} ${kochmorse_RCC_SOURCES})
target_link_libraries(kochmorse ${LIBS})

install(TARGETS kochmorse DESTINATION bin)


if (WITH_TEXTGEN)
 set(textgen_SOURCES textgen_cli.cc textgen.cc logger.cc globals.cc)
 set(textgen_MOC_HEADERS textgen.hh logger.hh)
 qt6_wrap_cpp(textgen_MOC_SOURCES ${textgen_MOC_HEADERS})

 add_executable(textgen WIN32 ${textgen_SOURCES} ${textgen_MOC_SOURCES})
 target_link_libraries(textgen ${LIBS})

 install(TARGETS textgen DESTINATION bin)
endif (WITH_TEXTGEN)

if (WITH_QSOCHAT)
 set(qsochat_SOURCES qsochat_cli.cc textgen.cc qsochat.cc)
 set(qsochat_MOC_HEADERS textgen.hh qsochat.hh)
 qt6_wrap_cpp(qsochat_MOC_SOURCES ${qsochat_MOC_HEADERS})

 add_executable(qsochat WIN32 ${qsochat_SOURCES} ${qsochat_MOC_SOURCES} ${kochmorse_RCC_SOURCES})
 target_link_libraries(qsochat ${LIBS})

 install(TARGETS qsochat DESTINATION bin)
endif (WITH_QSOCHAT)
