#ifndef __KOCHMORSE_EFFECT_HH__
#define __KOCHMORSE_EFFECT_HH__

#include "audiosink.hh"


/** Samples colored (filtered) noise and adds it to the clean signal. */
class NoiseEffect : public AudioSink
{
  Q_OBJECT

public:
  /** Constructor, snr specifies the signal-to-noise ratio. */
  explicit NoiseEffect(AudioSink *next, bool enabled=false, float snr=20, QObject *parent = 0);
  virtual ~NoiseEffect();

  virtual void play(const QByteArray &data);
  virtual double rate() const;

  void setEnabled(bool enabled);
  void setSNR(float snr);

protected:
  /** Samples two indp. std. normal distr. RV. */
  void gaussRNG(float &a, float &b);

protected:
  AudioSink *_sink;
  bool _enabled;
  float _snr;
};


/** Implements an alternating fading of the signal. */
class FadingEffect: public AudioSink
{
  Q_OBJECT

public:
  FadingEffect(AudioSink *sink, bool enabled=false, float maxDamp=-10, float rate=12, QObject *parent=0);
  virtual ~FadingEffect();

  virtual void play(const QByteArray &data);
  virtual double rate() const;

  void setEnabled(bool enabled);
  void setMaxDamp(float damp);
  void setFadingRate(float rate);

protected:
  AudioSink *_sink;
  bool _enabled;
  float _maxDamp;
  float _rate;
  int _dS;
  float _dF;
  float _factor;
};

#endif // __KOCHMORSE_EFFECT_HH__
