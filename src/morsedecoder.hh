#ifndef __KOCHMORSE_MORSEDECODER_HH__
#define __KOCHMORSE_MORSEDECODER_HH__

#include "audiosink.hh"
#include <QHash>


/** An annoyingly strict morse decoder.
 * This class implements a simple but relatively strict morse decoder as an @c AudioSink and
 * decodes received samples assuming a specified speed in terms of WPM. */
class MorseDecoder : public QIODevice
{
  Q_OBJECT

public:
  /** Constructs a Morse decoder for the specified speed.
   * The speed can be reset by a call @c setSpeed.
   * @param speed Specifies the expected speed in WPM.
   * @param threshold Specifies the signal detection threshold in RMS. */
  explicit MorseDecoder(double speed, float threshold=1e-5, QObject *parent = 0);
  /** Destructor. */
  virtual ~MorseDecoder();
  /** Reset the speed of the decoder in WPM. */
  void setSpeed(double wpm);
  /** Resets the theshold of the detector in RMS. */
  void setThreshold(double threshold);

signals:
  /** Gets emitted once an unknown char is received. */
  void unknownCharReceived(const QString &pattern);
  /** Gets emitted once a known char is received. */
  void charReceived(QChar ch);

protected:
  /** Reconfigures the decoder. */
  void _updateConfig();
  /** Returns true if the sample buffer contains a "dot". */
  bool _isDot() const;
  /** Returns true if the sample buffer contains a "dash". */
  bool _isDash() const;
  /** Returns true if the sample buffer contains an invalid symbol. */
  bool _isInvalid() const;
  /** Returns true if the sample buffer contains a "pause". */
  bool _isPause() const;

  /** Decodes symbols (dot, dash & pause) into chars. */
  void _processSymbol(char sym);

  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);

protected:
  /** Holds the expected speed in WPM. */
  double _speed;
  /** Holds the dit length in samples. */
  size_t _ditLength;
  /** Holds the unit length (1/4 of dit-length) in samples. */
  size_t _unitLength;
  /** The number of samples in the sample buffer. */
  size_t _Nsamples;
  /** The sample buffer. */
  int16_t *_buffer;
  /** Holds the detection threshold  in RMS. */
  float _threshold;
  /** The number of continous signal detections (1/4 dit-length). */
  size_t _highCount;
  /** The number of continous silence detections (1/4 dit-length). */
  size_t _lowCount;
  /** The number inter-char pauses received. */
  size_t _pauseCount;
  /** The decoded symbols ('-','.',' '). */
  QString _symbols;
  /** The last received char. */
  QChar _lastChar;
};

#endif // __KOCHMORSE_MORSEDECODER_HH__
