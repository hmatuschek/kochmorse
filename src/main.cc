#include "application.hh"
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
  Application app(argc, argv);
  MainWindow win(app);
  win.show();

  app.exec();

  return 0;
}
