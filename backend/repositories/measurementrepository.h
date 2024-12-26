#ifndef MEASUREMENTREPOSITORY_H
#define MEASUREMENTREPOSITORY_H
#include "../models/measurement.h"

class MeasurementRepository
{
public:
    MeasurementRepository();
    std::optional<Measurement> fetchById(qint64 id);
    QList<Measurement> getMeasurementsBySensorAndDate(qint64 sensorId, const QDateTime &startDate, const QDateTime &endDate);
    std::optional<Measurement> createMeasurement(const QByteArray& data, qint64 sensorId);
    void saveMeasurementToDatabase(const QByteArray& message);
};

#endif // MEASUREMENTREPOSITORY_H
