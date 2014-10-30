#include "audiosink.hh"
#include "globals.hh"
#include <iostream>


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
 * PortAudio, static interface
 * ********************************************************************************************* */
void
PortAudio::init() {
  Pa_Initialize();
}

void
PortAudio::finalize() {
  Pa_Terminate();
}


/* ********************************************************************************************* *
 * PortAudioSink, playback
 * ********************************************************************************************* */
PortAudioSink::PortAudioSink(QObject *parent)
  : AudioSink(parent), _stream(0), _volumeFactor(1)
{
  PaSampleFormat fmt = paInt16;
  size_t n_chanels = 1;
  Pa_OpenDefaultStream(&_stream, 0, n_chanels, fmt, (unsigned int)Globals::sampleRate, 2048, 0, 0);
  if (0 != _stream) { Pa_StartStream(_stream); }
}

PortAudioSink::~PortAudioSink() {
  if (0 != _stream) {
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
  }
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
PortAudioSink::volume() const {
  return _volumeFactor;
}

void
PortAudioSink::setVolume(double factor) {
  _volumeFactor = factor;
}


/* ********************************************************************************************* *
 * PortAudioSource, recording
 * ********************************************************************************************* */
PortAudioSource::PortAudioSource(AudioSink *sink, QObject *parent)
  : QObject(parent), _stream(0), _sink(sink)
{
  PaDeviceIndex dev_idx = Pa_GetDefaultInputDevice();
  PaStreamParameters params;
  params.channelCount = 1;
  params.device = dev_idx;
  params.sampleFormat = paInt16;
  params.suggestedLatency = Pa_GetDeviceInfo(dev_idx)->defaultLowInputLatency;
  params.hostApiSpecificStreamInfo = 0;

  Pa_OpenStream(&_stream, &params, 0, Globals::sampleRate, 0, paNoFlag,
                &PortAudioSource::_pa_callback, this);
  _buffer.reserve(2048*sizeof(int16_t));
}

PortAudioSource::~PortAudioSource() {
  if (0 != _stream)  {
    stop(); Pa_CloseStream(_stream);
  }
}

void
PortAudioSource::start() {
  if (! Pa_IsStreamActive(_stream)) {
    //std::cerr << "PA: Start RX." << std::endl;
    Pa_StartStream(_stream);
  }
}

void
PortAudioSource::stop() {
  if (Pa_IsStreamActive(_stream)) {
    //std::cerr << "PA: Stop RX." << std::endl;
    Pa_StopStream(_stream);
  }
}

bool
PortAudioSource::isRunning() const {
  return Pa_IsStreamActive(_stream);
}

int
PortAudioSource::_pa_callback(const void *in, void *out, unsigned long Nframes,
                              const PaStreamCallbackTimeInfo *tinfo, PaStreamCallbackFlags flags,
                              void *ctx)
{
  PortAudioSource *self = reinterpret_cast<PortAudioSource *>(ctx);
  int16_t *data = reinterpret_cast<int16_t *>(self->_buffer.data());
  memcpy(data, in, Nframes*sizeof(int16_t));
  self->_sink->play(QByteArray::fromRawData(self->_buffer.data(), Nframes*sizeof(int16_t)));
  return paContinue;
}
