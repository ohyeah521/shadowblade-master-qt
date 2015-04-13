#include "data.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDateTime>
#include <QUuid>

void Util::Data::saveData(QString sessionName, QByteArray data)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    QJsonObject::iterator it = jsonObject.find(sessionName);
    if(it == jsonObject.end())
    {
        return;
    }
    QJsonDocument jsonDocument;
    jsonDocument.setArray(it.value().toArray());
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_" + QUuid::createUuid().toString() + ".json";
    QDir().mkpath(sessionName);
    QFile file(sessionName + "/" + fileName);
    file.open(QFile::WriteOnly|QFile::Text);
    QTextStream stream(&file);
    stream << jsonDocument.toJson();
    file.flush();
    file.close();
}
