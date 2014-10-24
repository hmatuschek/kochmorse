#ifndef __KOCHMORSE_EFFECT_HH__
#define __KOCHMORSE_EFFECT_HH__

#include "audiosink.hh"

/** Samples colored (filtered) noise and adds it to the clean signal. */
class NoiseEffect : public AudioSink
{
  Q_OBJECT

public:
  /** Constructor, snr specifies the signal-to-noise ratio. */
  explicit NoiseEffect(AudioSink *next, bool enabled=false, double snr=20, QObject *parent = 0);
  virtual ~NoiseEffect();

  virtual void play(const QByteArray &data);

  virtual double rate() const;

protected:
  /** Samples two indp. std. normal distr. RV. */
  void gaussRNG(double &a, double &b) const;

protected:
  AudioSink *_sink;
  bool _enabled;
  double _snr;
};

#endif // __KOCHMORSE_EFFECT_HH__
