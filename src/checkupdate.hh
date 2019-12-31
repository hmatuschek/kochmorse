#ifndef CHECKUPDATE_HH
#define CHECKUPDATE_HH

#include <QObject>
#include <QNetworkAccessManager>


class CheckUpdate : public QObject
{
  Q_OBJECT

public:
	explicit CheckUpdate(QObject *parent = nullptr);

public slots:
	void check();

signals:
	void updateAvailable(QString name);

protected slots:
	void gotReleases(QNetworkReply *reply);

protected:
	QNetworkAccessManager *_network;
};

#endif // CHECKUPDATE_HH
