#include <iostream>
#include <QApplication>
#include "decoderwindow.hh"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  PortAudio::init();

  DecoderWindow win;
  win.show();
  app.exec();

  PortAudio::finalize();

  return 0;
}
