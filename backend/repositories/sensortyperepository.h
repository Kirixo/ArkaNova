#ifndef SENSORTYPEREPOSITORY_H
#define SENSORTYPEREPOSITORY_H
#include "../models/sensortype.h"
#include <qsqlquery.h>

class SensorTypeRepository
{
public:
    SensorTypeRepository() {};

    std::optional<SensorType> fetchById(qint64 id);
};

#endif // SENSORTYPEREPOSITORY_H
