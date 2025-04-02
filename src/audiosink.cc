#include "audiosink.hh"
#include "globals.hh"
#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include "logger.hh"


/* ********************************************************************************************* *
 * KAudioSink, playback
 * ********************************************************************************************* */
KAudioSink::KAudioSink(QIODevice *src, QObject *parent)
  : QIODevice(parent), _output(nullptr), _source(nullptr), _volume(1.0)
{
  this->open(QIODevice::ReadOnly);
  setSource(src);
}

KAudioSink::~KAudioSink() {
  if (this->isOpen())
    this->close();
  _output->stop();
}

double
KAudioSink::volume() const {
  return _volume;
}

void
KAudioSink::setVolume(double volume) {
  _volume = std::max(0.0, std::min(volume, 1.0));
  if (_output)
    _output->setVolume(_volume);
}

void
KAudioSink::setOutputDevice(const QAudioDevice &output_device) {
  if (_output && _output_device == output_device)
    return;

  if (_output) {
    _output->stop();
    delete _output;
    _output = nullptr;
  }

  QAudioFormat fmt;
  //fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  //fmt.setBytesPerSample(2);
  fmt.setSampleFormat(QAudioFormat::Int16);
  //fmt.setCodec("audio/pcm");
  QAudioDevice info(output_device);
  if (! info.isFormatSupported(fmt)) {
    logError() << "Audio format is not supported by device" << info.description();
  }

  _output_device = output_device;
  _output = new QAudioSink(_output_device, fmt);
  _output->setVolume(_volume);
  _output->start(this);
}

void
KAudioSink::setSource(QIODevice *source) {
  if (_source)
    disconnect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

  _source = source;
  if (! _source)
    return;

  connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  _source->open(QIODevice::ReadOnly);
}

bool
KAudioSink::isSequential() const {
  return true;
}

qint64
KAudioSink::bytesAvailable() const {
  if (! _source)
    return QIODevice::bytesAvailable();
  return _source->bytesAvailable() + QIODevice::bytesAvailable();
}

qint64
KAudioSink::readData(char *data, qint64 maxlen) {
  if (! _source)
    return 0;

  maxlen = _source->read(data, maxlen);
  return maxlen;
}

qint64
KAudioSink::writeData(const char *data, qint64 len) {
  Q_UNUSED(data);
  Q_UNUSED(len);
  return 0;
}

/* ********************************************************************************************* *
 * KAudioSource, recording
 * ********************************************************************************************* */
KAudioSource::KAudioSource(QIODevice *sink, QObject *parent)
  : QIODevice(parent), _input(nullptr), _sink(sink)
{
}

KAudioSource::~KAudioSource() {
  this->stop();
  this->close();
}

void
KAudioSource::start() {
  if (_input && QAudio::ActiveState != _input->state())
    _input->start(this);
}

void
KAudioSource::stop() {
  if (_input)
    _input->stop();
}

void
KAudioSource::setInputDevice(const QAudioDevice &input_device) {
  if (_input && _input_device == input_device)
    return;

  if (_input) {
    this->stop();
    delete _input;
    _input = nullptr;
  }

  QAudioFormat fmt;
  //fmt.setByteOrder(QSysInfo::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleFormat(QAudioFormat::Int16);
  //fmt.setBytesPerSample(2);
  //fmt.setCodec("audio/pcm");

  QAudioDevice info(input_device);
  if (! info.isFormatSupported(fmt)) {
    logWarn() << "Audio format is not supported by device" << info.description();
  }

  _input_device = input_device;
  _input = new QAudioSource(_input_device, fmt);
  this->open(QIODevice::WriteOnly);
  _buffer.reserve(2048*sizeof(int16_t));
}

bool
KAudioSource::isRunning() const {
  return QAudio::ActiveState == _input->state();
}

qint64
KAudioSource::readData(char *data, qint64 maxlen) {
  return 0;
}

qint64
KAudioSource::writeData(const char *data, qint64 len) {
  _buffer.append(data, len);
  _sink->write(_buffer);
  _buffer.clear();
  return len;
}
