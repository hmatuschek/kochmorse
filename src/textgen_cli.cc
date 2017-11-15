#include <QCoreApplication>
#include "textgen.hh"
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>


int
main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QTextStream out(stdout);
	QTextStream err(stderr);

	if (2 > argc) {
		err << argv[0] << " textgen RULE-FILE" << endl;
		return -1;
	}

  TextGen gen(argv[1]);
  QHash<QString, QString> ctx;
  gen.generate(out, ctx);
  out << flush;

  return 0;
}
