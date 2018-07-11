#include "audiosink.hh"
#include "globals.hh"
#include <iostream>


/* ********************************************************************************************* *
 * AudioSink, base class
 * ********************************************************************************************* */
AudioSink::AudioSink()
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
  : QObject(parent), AudioSink(), _stream(0), _volumeFactor(1)
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
PortAudioSink::process(const QByteArray &data) {
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
 * QAudioSink, playback
 * ********************************************************************************************* */
QAudioSink::QAudioSink(QObject *parent)
  : QIODevice(parent), AudioSink(), _output(0)
{
  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleSize(16);
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setCodec("audio/pcm");

  _output = new QAudioOutput(fmt, this);
  this->open(QIODevice::ReadOnly);
  _output->start(this);
}

QAudioSink::~QAudioSink() {
  _output->stop();
}

void
QAudioSink::process(const QByteArray &data) {
  bool empty = _buffer.isEmpty();
  _buffer.append(data);
  if (empty)
    emit readyRead();
}

double
QAudioSink::volume() const {
  return _output->volume();
}

void
QAudioSink::setVolume(double factor) {
  return _output->setVolume(factor);
}

qint64
QAudioSink::readData(char *data, qint64 maxlen) {
  maxlen = std::min(int(maxlen), _buffer.size());
  if (0 >= maxlen)
    return maxlen;

  memcpy(data, _buffer.data_ptr(), maxlen);
  _buffer.remove(0, maxlen);
  emit bytesWritten(maxlen);
  return maxlen;
}

qint64
QAudioSink::writeData(const char *data, qint64 len) {
  return 0;
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
  self->_sink->process(QByteArray::fromRawData(self->_buffer.data(), Nframes*sizeof(int16_t)));
  return paContinue;
}



/* ********************************************************************************************* *
 * QAudioSource, recording
 * ********************************************************************************************* */
QAudioSource::QAudioSource(AudioSink *sink, QObject *parent)
  : QIODevice(parent), _stream(0), _sink(sink)
{
  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setSampleSize(16);
  fmt.setCodec("audio/pcm");
  _stream = new QAudioInput(fmt, this);
  this->open(QIODevice::WriteOnly);
  _buffer.reserve(2048*sizeof(int16_t));
}

QAudioSource::~QAudioSource() {
  if (0 != _stream)
    this->stop();
  this->close();
}

void
QAudioSource::start() {
  if (QAudio::ActiveState != _stream->state())
    _stream->start(this);
}

void
QAudioSource::stop() {
  _stream->stop();
}

bool
QAudioSource::isRunning() const {
  return QAudio::ActiveState == _stream->state();
}

qint64
QAudioSource::readData(char *data, qint64 maxlen) {
  return 0;
}

qint64
QAudioSource::writeData(const char *data, qint64 len) {
  _buffer.append(data, len);
  _sink->process(_buffer);
  _buffer.clear();
  return len;
}
