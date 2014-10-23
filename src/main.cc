#include "application.hh"
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
  Application app(argc, argv);

  MainWindow win(app);
  win.show();

  app.exec();

  app.stopSession();

  return 0;
}
