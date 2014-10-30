#ifndef __KOCHMORSE_MORSEDECODER_HH__
#define __KOCHMORSE_MORSEDECODER_HH__

#include "audiosink.hh"
#include <QHash>


/** An annoyingly strict morse decoder. */
class MorseDecoder : public AudioSink
{
  Q_OBJECT

public:
  explicit MorseDecoder(double speed, double freq, QObject *parent = 0);
  virtual ~MorseDecoder();

  void setFreq(double freq);
  void setSpeed(double wpm);
  void setThreshold(double threshold);

  void play(const QByteArray &data);

signals:
  void detectedSignal(int sig);
  void unknownCharReceived(const QString &pattern);
  void charReceived(QChar ch);

protected:
  void _updateConfig();
  bool _isDot() const;
  bool _isDash() const;
  bool _isPause() const;

  void _processSymbol(char sym);

protected:
  double _speed;
  double _freq;

  size_t _unitLength;

  size_t _Nsamples;
  int16_t *_buffer;

  float *_sinTable;
  float *_cosTable;

  float _threshold;

  size_t _delaySize;
  int16_t _delayLine[8];

  size_t _pauseCount;
  QString _symbols;

  QChar _lastChar;
};

#endif // __KOCHMORSE_MORSEDECODER_HH__
