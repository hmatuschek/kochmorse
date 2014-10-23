#include "audiosink.hh"
#include <iostream>


AudioSink::AudioSink(QObject *parent, double sampleRate)
  : QObject(parent), _stream(0), _rate(sampleRate)
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
  Pa_WriteStream(_stream, data.constData(), data.size()/2);
}

double
AudioSink::rate() const {
  return _rate;
}
