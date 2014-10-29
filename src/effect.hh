#ifndef __KOCHMORSE_EFFECT_HH__
#define __KOCHMORSE_EFFECT_HH__

#include "audiosink.hh"


/** Samples colored (filtered) noise and adds it to the clean signal. */
class NoiseEffect : public AudioSink
{
  Q_OBJECT

public:
  /** Constructor, @c snr specifies the signal-to-noise ratio. */
  explicit NoiseEffect(AudioSink *next, bool enabled=false, float snr=20, QObject *parent = 0);
  /** Destructor. */
  virtual ~NoiseEffect();
  /** Processes the given data. */
  virtual void play(const QByteArray &data);
  /** Returns the sample-rate in Hz. */
  virtual double rate() const;
  /** Enable/Disable the effect. */
  void setEnabled(bool enabled);
  /** Sets the SNR. */
  void setSNR(float snr);

protected:
  /** Samples two indp. std. normal distr. RV. */
  void gaussRNG(float &a, float &b);

protected:
  /** The audio sink of the effect. */
  AudioSink *_sink;
  /** If @c true, the effect instance adds some noise. */
  bool _enabled;
  /** The current SNR. */
  float _snr;
};


/** Implements an alternating fading of the signal. */
class FadingEffect: public AudioSink
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the @c AudioSink of the effect.
   * @param enabled Specified whether the effect is enabled.
   * @param maxDamp Specifies the maximum damping factor (in dB) of the fading effect.
   * @param rate Specifies the rate of the fading in [1/min].
   * @param parent Specifies the QObject parent. */
  FadingEffect(AudioSink *sink, bool enabled=false, float maxDamp=-10, float rate=12, QObject *parent=0);
  /** Destructor. */
  virtual ~FadingEffect();
  /** Processes the input audio data. */
  virtual void play(const QByteArray &data);
  /** Retunrs the sample-rate. */
  virtual double rate() const;
  /** Enable/disable the effect. */
  void setEnabled(bool enabled);
  /** (Re-) Set the maximum damping factor. */
  void setMaxDamp(float damp);
  /** (Re-) Set the fading rate [1/min]. */
  void setFadingRate(float rate);

protected:
  /** The audio sink of the effect. */
  AudioSink *_sink;
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
