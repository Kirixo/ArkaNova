#include "sensor.h"

Sensor::Sensor()
    : id_(-1), solarPanel_(), type_()
{
}

Sensor::Sensor(qint64 id, const SolarPanel &solarPanel, const SensorType&  type)
    : id_(id), solarPanel_(solarPanel), type_(type)
{
}

Sensor::Sensor(const Sensor &sensor)
    : id_(sensor.id_), solarPanel_(sensor.solarPanel_), type_(sensor.type_)
{

}

Sensor::Sensor(Sensor &&sensor)
    : id_(std::move(sensor.id_)), solarPanel_(std::move(sensor.solarPanel_)), type_(std::move(sensor.type_))
{
}

qint64 Sensor::id() const
{
    return id_;
}

void Sensor::setId(qint64 newId)
{
    id_ = newId;
}

const SensorType& Sensor::type() const
{
    return type_;
}

void Sensor::setType(const SensorType &newType)
{
    type_ = newType;
}

const SolarPanel& Sensor::solarPanel() const
{
    return solarPanel_;
}

void Sensor::setSolarPanel(const SolarPanel &newSolarPanel)
{
    solarPanel_ = newSolarPanel;
}

QJsonObject Sensor::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["type"] = type_.toJson();
    json["solar_panel_id"] = solarPanel_.id();
    return json;
}
