#include <QCoreApplication>
#include "qsochat.hh"
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>


int
main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QTextStream out(stdout);
	QTextStream in(stdin);

  QSOChat chat;
  QString input;
  QString response;
  QTextStream buffer(&response);
  while (true) {
    response.clear();
    input = in.readLine();
    chat.handle(input, buffer);
    out << response << endl;
    qDebug() << chat.context();
  }

  return 0;
}
