#include "checkupdate.hh"
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>

#include "config.h"

CheckUpdate::CheckUpdate(QObject *parent)
  : QObject(parent), _network(nullptr)
{
  _network = new QNetworkAccessManager(this);
  connect(_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotReleases(QNetworkReply*)));
}

void
CheckUpdate::check() {
  _network->get(QNetworkRequest(QUrl("https://api.github.com/repos/hmatuschek/kochmorse/releases")));
}

void
CheckUpdate::gotReleases(QNetworkReply *reply) {
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &err);
  if (QJsonParseError::NoError != err.error) {
    qDebug() << "Cannot check for updates at" << reply->url() << ":" << err.errorString();
    return;
  }

  if (! doc.isArray()) {
    qDebug() << "Cannot check for updates at" << reply->url() << ": Unexpected JSON object.";
    return;
  }

  QJsonObject obj = doc.array().first().toObject();
  QString myVersion = QString("v%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH);
  QString remVersion = obj.find("tag_name").value().toString();
  QString versionName = obj.find("name").value().toString();
  if (myVersion < remVersion)
    emit updateAvailable(versionName);
}
