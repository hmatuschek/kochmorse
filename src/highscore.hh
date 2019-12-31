#ifndef HIGHSCORE_HH
#define HIGHSCORE_HH

#include <QObject>
#include <QJsonObject>
#include <QDateTime>
#include <QNetworkAccessManager>

class HighScore : public QObject
{
  Q_OBJECT

public:
	class Entry {
	public:
		QDateTime time;
		QString tutor;
		int wpm;
		int effWpm;
		int lesson;
		int score;
	public:
		Entry(const QJsonObject &obj);
		Entry(QString t, int w, int e, int l, int s);
		QJsonObject serialize() const;
	};

public:
	explicit HighScore(QObject *parent = nullptr);

public slots:
	void emitScore(const QString &tutor, int wpm, int effWpm, int lesson, int score);
	void upload();

protected slots:
	void serialize() const;
	void onUploadFinished(QNetworkReply *reply);

protected:
	QList<Entry> _entries;
	QNetworkAccessManager *_net;
};

#endif // HIGHSCORE_HH
