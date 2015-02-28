#ifndef __KOCHMORSE_MORSEDECODER_HH__
#define __KOCHMORSE_MORSEDECODER_HH__

#include "audiosink.hh"
#include <QHash>


/** An annoyingly strict morse decoder. */
class MorseDecoder : public AudioSink
{
  Q_OBJECT

public:
  explicit MorseDecoder(double speed, QObject *parent = 0);
  virtual ~MorseDecoder();

  void setSpeed(double wpm);
  void setThreshold(double threshold);

  void process(const QByteArray &data);

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

  size_t _unitLength;

  size_t _Nsamples;
  int16_t *_buffer;

  float _threshold;

  size_t _delaySize;
  bool _delayLine[8];

  size_t _pauseCount;
  QString _symbols;

  QChar _lastChar;
};

#endif // __KOCHMORSE_MORSEDECODER_HH__
