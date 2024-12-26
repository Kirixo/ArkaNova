#include "sensortype.h"

SensorType::SensorType()
    : id_(-1), name_(QString())
{
}

SensorType::SensorType(qint64 id, const QString &name)
    : id_(id), name_(name)
{
}

SensorType::SensorType(const SensorType &sensorType)
    : id_(sensorType.id_), name_(sensorType.name_)
{
}

SensorType::SensorType(SensorType &&sensorType)
    : id_(std::move(sensorType.id_)), name_(std::move(sensorType.name_))
{
}

qint64 SensorType::id() const
{
    return id_;
}

void SensorType::setId(qint64 newId)
{
    id_ = newId;
}

const QString& SensorType::name() const
{
    return name_;
}

void SensorType::setName(const QString &newName)
{
    name_ = newName;
}

QJsonObject SensorType::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["name"] = name_;
    return json;
}
