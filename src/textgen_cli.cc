#include <QCoreApplication>
#include "textgen.hh"
#include "globals.hh"
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>


int
main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QTextStream err(stderr);

	if (2 > argc) {
		err << argv[0] << " textgen RULE-FILE" << endl;
		return -1;
	}

  TextGen gen(argv[1]);

  QString text;
  QTextStream buffer(&text);

  QHash<QString, QString> ctx;
  gen.generate(buffer, ctx);

  foreach (QChar prosign, Globals::prosignTable.keys()) {
    text.replace(prosign, "<"+Globals::prosignTable[prosign]+">");
  }

  QTextStream out(stdout);
  out << text << flush;

  return 0;
}
