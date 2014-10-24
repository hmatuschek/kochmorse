#include "application.hh"
#include "mainwindow.hh"

// The main routine...
int main(int argc, char *argv[])
{
  Application app(argc, argv);

  MainWindow win(app);
  win.show();

  // Main loop
  app.exec();

  // Wait for the MorseEncoder thread to quit
  app.stopSession();

  return 0;
}
