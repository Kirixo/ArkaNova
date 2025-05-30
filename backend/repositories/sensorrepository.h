#ifndef SENSORREPOSITORY_H
#define SENSORREPOSITORY_H
#include "../models/sensor.h"
#include "sensortyperepository.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>

class SensorRepository {
public:
    std::optional<Sensor> getSensorById(qint64 id);
    QList<Sensor> getSensorsByPanelId(qint64 id);
    bool deleteSensor(qint64 id);
    std::optional<Sensor> createSensor(const Sensor& sensor);
};


#endif // SENSORREPOSITORY_H
