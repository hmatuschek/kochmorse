#include "audiosink.hh"
#include <iostream>


AudioSink::AudioSink(double sampleRate, QObject *parent)
  : QObject(parent), _stream(0), _rate(sampleRate), _volumeFactor(1)
{
  PaSampleFormat fmt = paInt16;
  size_t n_chanels = 1;
  Pa_OpenDefaultStream(&_stream, 0, n_chanels, fmt, (unsigned int)_rate, 2048, 0, 0);
  if (0 != _stream) { Pa_StartStream(_stream); }
}

AudioSink::~AudioSink() {
  if (0 != _stream) {
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
  }
}

void
AudioSink::init() {
  Pa_Initialize();
}

void
AudioSink::finalize() {
  Pa_Terminate();
}

void
AudioSink::play(const QByteArray &data) {
  QByteArray buffer(data);
  int16_t *s = reinterpret_cast<int16_t *>(buffer.data());
  for (int i=0; i<data.size()/2; i++) {
    s[i] *= _volumeFactor;
  }
  Pa_WriteStream(_stream, buffer.constData(), buffer.size()/2);
}

double
AudioSink::rate() const {
  return _rate;
}

double
AudioSink::volume() const {
  return _volumeFactor;
}

void
AudioSink::setVolume(double factor) {
  _volumeFactor = factor;
}
