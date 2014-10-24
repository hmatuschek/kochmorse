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

#endif // __KOCHMORSE_EFFECT_HH__
