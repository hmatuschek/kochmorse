#ifndef QRM_HH
#define QRM_HH

#include <QObject>
#include <QIODevice>
#include <QBuffer>
#include "morseencoder.hh"
#include "tutor.hh"

/** This class implements a rather complex QRM effect by mixing the audio signal with
 * additional audio streams of several parallel QSOs. */
class QRMGenerator: public QIODevice
{
	Q_OBJECT

public:
  /** Constructs a QRM audio effect.
   * @param source Specifies the audio source.
   * @param num Specifies the number of QRM stations.
   * @param snr Specifies the signal-to-qrm ratio in dB.
   * @param parent Specifies the QObject parent. */
	QRMGenerator(QIODevice *source, size_t num, double snr, QObject *parent=nullptr);
  /** Destructor. */
	virtual ~QRMGenerator();

  /** Returns  @c true if enabled. */
	bool enabled() const;
  /** Enables/Disables the QRM effect. */
	void enable(bool enabled);

  /** Returns the number of QRM stations. */
  int stations() const;
  /** Sets the number of QRM stations. */
  void setStations(int num);

  /** Returns the signal-to-QRM ratio. */
  double snr() const;
  /** Sets the signal-to-QRM ratio. */
  void setSNR(double db);

  /** Sets the audio source. */
  void setSource(QIODevice *src);

  /** Returns the number of bytes ready to be read. */
  qint64 bytesAvailable() const;

protected:
  /** Does nothing. Implements the QIODevice interface. */
	qint64 writeData(const char *data, qint64 len);
  /** Reads some data. */
	qint64 readData(char *data, qint64 maxlen);

protected:
  /** The audio source. */
	QIODevice *_source;
  /** If @c true, the QRM effect is enabled. */
	bool _enabled;
  /** The number of stations. */
	size_t _num;
  /** An independent morse encoder for each QRM station. */
	QVector<MorseEncoder *> _encoder;
  /** An independent text generator for each QRM station. */
	QVector<GenTextTutor *> _generator;
  /** Holds the signal-to-qrm ratio. */
  double _snr;
  /** Signal scaling factor. */
  double _fsig;
  /** QRM scaling factor. */
  double _fqrm;
};



#endif // QRM_HH
