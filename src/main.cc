#include "application.hh"
#include "mainwindow.hh"
#include "logger.hh"

// The main routine...
int main(int argc, char *argv[])
{
  QTextStream err_str(stderr);

  Logger::get().addHandler(new StreamLogHandler(err_str, LogMessage::INFO));

  Application app(argc, argv);

  MainWindow win(app);
  win.show();

  // Main loop
  app.exec();

  // Wait for the MorseEncoder thread to quit
  app.stopSession();

  return 0;
}
