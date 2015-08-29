#include "data.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

void Util::Data::saveData(QString sessionName, QByteArray data)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    QJsonDocument jsonDocument;
    jsonDocument.setArray(jsonObject.take(sessionName).toArray());
    QString imei = jsonObject.take("imei").toString("null");
    QString imsi = jsonObject.take("imsi").toString("null");
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_imei_" + imei + "_imsi_" + imsi + ".json";
    QDir().mkpath(sessionName);
    QFile file(sessionName + "/" + fileName);
    file.open(QFile::WriteOnly|QFile::Text);
    QTextStream stream(&file);
    stream << jsonDocument.toJson();
    file.flush();
    file.close();
}
