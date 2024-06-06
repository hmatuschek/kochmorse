#include "ringbuffer.hh"
#include <limits>

RingBuffer::RingBuffer(unsigned int size, QObject *parent)
  : QIODevice{parent}, _data(size, 0), _readIndex(0), _writeIndex(0)
{
  // pass...
}

bool
RingBuffer::bufferIsFull() const {
  return std::numeric_limits<unsigned int>::max() == _readIndex;
}

void
RingBuffer::clearBuffer() {
  _writeIndex = 0;
  _readIndex = 0;
}

unsigned int
RingBuffer::bytesInBuffer() const {
  // full
  if (bufferIsFull())
    return _data.size();

  if (_writeIndex > _readIndex)
    return _writeIndex - _readIndex;

  return _writeIndex + _data.size() - _readIndex;
}

bool
RingBuffer::isSequential() const {
  return true;
}

qint64
RingBuffer::bytesAvailable() const {
  return bytesInBuffer() + QIODevice::bytesAvailable();
}

qint64
RingBuffer::bytesToWrite() const {
  return bytesInBuffer() + QIODevice::bytesToWrite();
}

qint64
RingBuffer::readData(char *data, qint64 maxlen) {
  maxlen = std::min(qint64(bytesInBuffer()), maxlen);

  if (maxlen && bufferIsFull())
    _readIndex = _writeIndex;

  for (qint64 i=0; i<maxlen; i++) {
    data[i] = _data.at(_readIndex);
    _readIndex++;
    if ((qint64)_readIndex >= _data.size())
      _readIndex = 0;
  }

  if (maxlen)
    emit bytesWritten(maxlen);

  return maxlen;
}

qint64
RingBuffer::writeData(const char *data, qint64 len) {
  for (qint64 i=0; i<len; i++) {
    _data.data()[_writeIndex] = data[i];

    _writeIndex++;
    if ((qint64)_writeIndex >= _data.size())
      _writeIndex = 0;

    if (_readIndex == _writeIndex)
      _readIndex = std::numeric_limits<unsigned int>::max();
  }

  if (bytesAvailable())
    emit readyRead();

  return len;
}
