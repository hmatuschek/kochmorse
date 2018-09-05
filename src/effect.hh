#ifndef __KOCHMORSE_EFFECT_HH__
#define __KOCHMORSE_EFFECT_HH__

#include "audiosink.hh"

#define FIR_ORDER 32

/** Samples colored (filtered) noise and adds it to the clean signal. */
class NoiseEffect : public QIODevice
{
  Q_OBJECT

public:
  /** Constructor, @c snr specifies the signal-to-noise ratio. */
  explicit NoiseEffect(QIODevice *src=nullptr, bool enabled=false, float snr=20, bool filter=false,
                       float Fc=650, float Bw=300, QObject *parent = nullptr);
  /** Destructor. */
  virtual ~NoiseEffect();

  /** Enable/Disable the effect. */
  void setEnabled(bool enabled);
  /** Sets the SNR. */
  void setSNR(float snr);

  /** Returns @c true if the filter is enabled. */
  bool filterEnabled() const;
  /** Enables/disables the filter. */
  void setFiterEnabled(bool enabled);

  /** Returns the center frequency of the filter. */
  float Fc() const;
  /** Sets the filters center frequency. */
  void setFc(float Fc);

  /** Returns the filter bandwidth. */
  float Bw() const;
  /** Sets the filter bandwidth. */
  void setBw(float Bw);

  /** Sets the audio source. */
  void setSource(QIODevice *src);

  /** Retruns the number of bytes available for reading. Implements the QIODevice interface. */
  qint64 bytesAvailable() const;

protected:
  /** Samples two indp. std. normal distr. RV. */
  void gaussRNG(float &a, float &b);
  /** Apply the FIR band-pass filter to the sample. */
  float filter(float value);
  /** Updates the FIR band-pass filter. */
  void updateFIR();
  /** Does nothing. Implements the QIODevice interface. */
  qint64 writeData(const char *data, qint64 len);
  /** Reads some data. Implements the QIODevice interface. */
  qint64 readData(char *data, qint64 maxlen);

protected:
  /** The audio source of the effect. */
  QIODevice *_source;
  /** If @c true, the effect instance adds some noise. */
  bool _enabled;
  /** The current SNR. */
  float _snr;
  /** Signal scaling factor. */
  float _sfac;
  /** Noise scaling factor. */
  float _nfac;
  /** If @c true, enables filtering. */
  bool _filter;
  /** FIR center frequency. */
  float _Fc;
  /** FIR filter bandwidth. */
  float _Bw;
  /** FIR filter kernel. */
  float _fir[FIR_ORDER];
  /** FIR convolution ring buffer. */
  float _buffer[FIR_ORDER];
  /** Ring buffer index. */
  int _bidx;
};


/** Implements an alternating fading of the signal. */
class FadingEffect: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param source Specifies the @c AudioSource of the effect.
   * @param enabled Specified whether the effect is enabled.
   * @param maxDamp Specifies the maximum damping factor (in dB) of the fading effect.
   * @param rate Specifies the rate of the fading in [1/min].
   * @param parent Specifies the QObject parent. */
  FadingEffect(QIODevice *source, bool enabled=false, float maxDamp=-10, float rate=12,
               QObject *parent=nullptr);
  /** Destructor. */
  virtual ~FadingEffect();
  /** Enable/disable the effect. */
  void setEnabled(bool enabled);
  /** (Re-) Set the maximum damping factor. */
  void setMaxDamp(float damp);
  /** (Re-) Set the fading rate [1/min]. */
  void setFadingRate(float rate);
  void setSource(QIODevice *src);

  /** Returns the number of bytes available for reading. Implements the QIODevice interface. */
  qint64 bytesAvailable() const;

protected:
  /** Reads some data. Implements the QIODevice interface. */
  qint64 readData(char *data, qint64 maxlen);
  /** Does nothing. Implements the QIODevice interface. */
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The audio source of the effect. */
  QIODevice *_source;
  /** If @c true, the effect is enabled. */
  bool _enabled;
  /** The maximum damping factor. */
  float _maxDamp;
  /** The damping rate in [1/min]. */
  float _rate;
  /** The number of samples to the next fading. */
  int _dS;
  /** The slope of the fading factor [1/sample]. */
  float _dF;
  /** The current fading factor [1, _maxDamp]. */
  float _factor;
};

#endif // __KOCHMORSE_EFFECT_HH__
