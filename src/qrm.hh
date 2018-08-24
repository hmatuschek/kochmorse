#ifndef QRM_HH
#define QRM_HH

#include <QObject>
#include <QIODevice>
#include <QBuffer>
#include "morseencoder.hh"
#include "tutor.hh"


class QRMGenerator: public QIODevice
{
	Q_OBJECT

public:
	QRMGenerator(QIODevice *source, size_t num, double snr, QObject *parent);
	virtual ~QRMGenerator();

	bool enabled() const;
	void enable(bool enabled);

  int stations() const;
  void setStations(int num);

  double snr() const;
  void setSNR(double db);

  void setSource(QIODevice *src);
  qint64 bytesAvailable() const;

protected:
	qint64 writeData(const char *data, qint64 len);
	qint64 readData(char *data, qint64 maxlen);

protected:
	QIODevice *_source;
	bool _enabled;

	size_t _num;
	QVector<MorseEncoder *> _encoder;
	QVector<GenTextTutor *> _generator;

  double _snr;
  double _fsig;
  double _fqrm;
};



#endif // QRM_HH
