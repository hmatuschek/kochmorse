#include "highscore.hh"
#include "settings.hh"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDir>


HighScore::HighScore(QObject *parent)
    : QObject(parent), _entries(), _net(0)
{
  _net = new QNetworkAccessManager(this);
  connect(_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(onUploadFinished(QNetworkReply*)));

  QString dirname(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  QDir dir = QDir(dirname);
  if (! dir.exists()) {
    if (! dir.mkpath(dirname))
      qDebug() << "Cannot create directory" << dir.path();
  }
	QString filename = dirname + "/highscore.json";
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		qDebug() << "Cannot open highscore file" << filename;
		return;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();
	if (! doc.isArray()) {
		qDebug() << "Cannot read highscore file" << filename << ": Not an array!";
		return;
	}

	QJsonArray entries = doc.array();
	foreach(QJsonValue entry, entries) {
		if (! entry.isObject())
			continue;
		_entries.push_back(Entry(entry.toObject()));
	}

  Settings settings;
  if (settings.sendHighScore())
    upload();
}

void
HighScore::emitScore(const QString &tutor, int wpm, int effWpm, int lesson, int score) {
  _entries.append(Entry(tutor, wpm, effWpm, lesson, score));
  serialize();

  Settings settings;
  if (settings.sendHighScore())
    upload();
}

void
HighScore::serialize() const{
  QJsonArray lst;
  foreach(Entry entry, _entries) {
    lst.append(entry.serialize());
  }
  QJsonDocument doc;
  doc.setArray(lst);

  QString filename = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
	        + "/highscore.json";
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		qDebug() << "Cannot open highscore file" << filename;
		return;
	}

  file.write(doc.toJson());
  file.flush();
  file.close();
}


void
HighScore::upload() {
  Settings settings;
  QString id = settings.hsID();
  QString call = settings.hsCall().simplified();

  if (! QRegExp("[0-9a-zA-Z]+").exactMatch(call))
    return;

  if (0 == _entries.size())
    return;

  Entry &entry = _entries.first();
  QUrlQuery query;
  query.addQueryItem("id",id);
  query.addQueryItem("call", call);
  query.addQueryItem("dt", entry.time.toString(Qt::ISODate));
  query.addQueryItem("tutor", entry.tutor);
  query.addQueryItem("wpm", QString::number(entry.wpm));
  query.addQueryItem("eff", QString::number(entry.effWpm));
  query.addQueryItem("lesson", QString::number(entry.lesson));
  query.addQueryItem("score", QString::number(entry.score));
  QUrl url("http://dm3mat.darc.de/kochmorse/log.php");
  url.setQuery(query);
  qDebug() << "Upload HS entry to" << url;
  _net->get(QNetworkRequest(url));
}

void
HighScore::onUploadFinished(QNetworkReply *reply) {
  if (QNetworkReply::NoError != reply->error()) {
    qDebug() << "Cannot upload HS:" << reply->errorString();
    return;
  }

  _entries.pop_front();
  serialize();

  if (_entries.size())
    upload();
}



HighScore::Entry::Entry(QString t, int w, int e, int l, int s)
  : time(QDateTime::currentDateTimeUtc()), tutor(t), wpm(w), effWpm(e), lesson(l), score(s)
{
  // pass...
}

HighScore::Entry::Entry(const QJsonObject &obj)
  : time(QDateTime::fromString(obj.value("time").toString(), Qt::ISODate)),
    tutor(obj.value("tutor").toString()), wpm(obj.value("wpm").toInt()),
    effWpm(obj.value("effWpm").toInt()), lesson(obj.value("lesson").toInt()),
    score(obj.value("score").toInt())
{
  // pass...
}


QJsonObject
HighScore::Entry::serialize() const {
  QJsonObject obj;
  obj.insert("time", time.toString(Qt::ISODate));
  obj.insert("tutor", tutor);
  obj.insert("wpm", wpm);
  obj.insert("effWpm", effWpm);
  obj.insert("lesson", lesson);
  obj.insert("score", score);
  return obj;
}

