#include "audiosink.hh"


/* ********************************************************************************************* *
 * AudioSink, base class
 * ********************************************************************************************* */
AudioSink::AudioSink(QObject *parent)
  : QObject(parent)
{
  // pass...
}

AudioSink::~AudioSink() {
  // pass...
}


/* ********************************************************************************************* *
 * PortAudioSink, playback
 * ********************************************************************************************* */
PortAudioSink::PortAudioSink(double sampleRate, QObject *parent)
  : AudioSink(parent), _stream(0), _rate(sampleRate), _volumeFactor(1)
{
  PaSampleFormat fmt = paInt16;
  size_t n_chanels = 1;
  Pa_OpenDefaultStream(&_stream, 0, n_chanels, fmt, (unsigned int)_rate, 2048, 0, 0);
  if (0 != _stream) { Pa_StartStream(_stream); }
}

PortAudioSink::~PortAudioSink() {
  if (0 != _stream) {
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
  }
}

void
PortAudioSink::init() {
  Pa_Initialize();
}

void
PortAudioSink::finalize() {
  Pa_Terminate();
}

void
PortAudioSink::play(const QByteArray &data) {
  QByteArray buffer(data);
  int16_t *s = reinterpret_cast<int16_t *>(buffer.data());
  for (int i=0; i<data.size()/2; i++) {
    s[i] *= _volumeFactor;
  }
  Pa_WriteStream(_stream, buffer.constData(), buffer.size()/2);
}

double
PortAudioSink::rate() const {
  return _rate;
}

double
PortAudioSink::volume() const {
  return _volumeFactor;
}

void
PortAudioSink::setVolume(double factor) {
  _volumeFactor = factor;
}
