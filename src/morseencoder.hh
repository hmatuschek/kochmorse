#ifndef __KOCHMORSE_MORSEENCODER_HH__
#define __KOCHMORSE_MORSEENCODER_HH__

#include <QObject>
#include <QHash>
#include "audiosink.hh"


class MorseEncoder : public QObject
{
  Q_OBJECT

public:
  explicit MorseEncoder(
      AudioSink *sink, double ditFreq, double daFreq, double speed, double effSpeed,
      QObject *parent= 0);

  void send(const QString &text);
  void send(QChar ch);

protected:
  static QHash<QChar, QString> _codeTable;

protected:
  double _ditFreq;
  double _daFreq;

  /** Char speed in WPM. */
  double _speed;
  /** Effective speed (pauses) in WPM. */
  double _effSpeed;

  /** Length of a "dit" in samples. */
  size_t _unitLength;
  /** Pause length in samples. */
  size_t _effUnitLength;

  /** A "dit" incl. inter-symbol pause. */
  QByteArray _ditSample;
  /** A "da" incl. inter-symbol pause. */
  QByteArray _daSample;
  /** Pause duration between chars. */
  QByteArray _icPause;
  /** Pause duration between words. */
  QByteArray _iwPause;
  /** Audio backend. */
  AudioSink *_sink;
};

#endif // __KOCHMORSE_MORSEENCODER_HH__
