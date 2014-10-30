#ifndef __KOCHMORSE_GLOBALS_HH__
#define __KOCHMORSE_GLOBALS_HH__

#include <QHash>

class Globals
{
public:
  /** The global audio sample-rate. */
  static const double sampleRate;

  /** The static morse-code mapping code to chars. */
  static QHash<QString, QChar> codeTable;
  /** The static morse-code mapping char to code. */
  static QHash<QChar, QString> charTable;
  /** Maps my prosign unicode chars to text representation. */
  static QHash<QChar, QString> prosignTable;

  static QString mapProsign(QChar ch);
};

#endif // __KOCHMORSE_GLOBALS_HH__
