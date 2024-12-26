#ifndef SENSOR_H
#define SENSOR_H

#include "../utils/jsonable.h"
#include "../models/sensortype.h"
#include "../models/solarpanel.h"

class Sensor : Jsonable
{
public:
    Sensor();
    Sensor(qint64 id, const SolarPanel& solarPanel, const SensorType& type);
    Sensor(const Sensor& sensor);
    Sensor(Sensor&& sensor);
    Sensor &operator=(const Sensor &) = default;
    Sensor &operator=(Sensor &&) = default;

    qint64 id() const;
    void setId(qint64 newId);
    const SensorType& type() const;
    void setType(const SensorType &newType);
    const SolarPanel& solarPanel() const;
    void setSolarPanel(const SolarPanel &newSolarPanel);

private:
    qint64 id_ {-1};
    SensorType type_ {};
    SolarPanel solarPanel_ {};

    // Jsonable interface
public:
    QJsonObject toJson() const override;


};

#endif // SENSOR_H
