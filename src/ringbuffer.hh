#ifndef RINGBUFFER_HH
#define RINGBUFFER_HH

#include <QIODevice>
#include <QObject>

class RingBuffer : public QIODevice
{
public:
  explicit RingBuffer(unsigned int size, QObject *parent = nullptr);

  bool isSequential() const;
  qint64 bytesAvailable() const;
  qint64 bytesToWrite() const;

protected:
  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);

private:
  bool bufferIsFull() const;
  void clearBuffer();
  unsigned int bytesInBuffer() const;

private:
  QByteArray _data;
  unsigned int _readIndex, _writeIndex;
};

#endif // RINGBUFFER_HH
