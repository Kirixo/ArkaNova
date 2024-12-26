#include "measurementrepository.h"
#include "../controllers/dbcontroller.h"
#include "sensorrepository.h"
#include <qdatetime.h>
#include <qsqlerror.h>

MeasurementRepository::MeasurementRepository() {}

std::optional<Measurement> MeasurementRepository::fetchById(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, data, recorded_at, sensor_id
        FROM measurement
        WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        SensorRepository sensorRepository;
        qint64 measurementId = query.value("id").toLongLong();
        QByteArray data = query.value("data").toByteArray();
        QDateTime recordedAt = query.value("recorded_at").toDateTime();
        auto sensor = sensorRepository.getSensorById(query.value("sensor_id").toLongLong());
        if(!sensor) {
            return std::nullopt;
        }

        return Measurement(measurementId, data, recordedAt, sensor.value());
    }

    qDebug() << "Database error while fetching Measurement by ID:" << query.lastError().text();
    return std::nullopt;
}

QList<Measurement> MeasurementRepository::getMeasurementsBySensorAndDate(qint64 sensorId,
                                                                         const QDateTime& startDate,
                                                                         const QDateTime& endDate) {
    QList<Measurement> measurements;
    QSqlQuery query(DBController::getDatabase());

    QString queryString = R"(
        SELECT id, data, recorded_at, sensor_id
        FROM measurement
        WHERE sensor_id = :sensor_id
    )";

    if (!startDate.isNull()) {
        queryString += " AND recorded_at >= :start_date";
    }

    if (!endDate.isNull()) {
        queryString += " AND recorded_at <= :end_date";
    }

    queryString += " ORDER BY recorded_at DESC";

    query.prepare(queryString);
    query.bindValue(":sensor_id", sensorId);

    if (!startDate.isNull()) {
        query.bindValue(":start_date", startDate.toString(Qt::ISODate));
    }

    if (!endDate.isNull()) {
        query.bindValue(":end_date", endDate.toString(Qt::ISODate));
    }

    if (query.exec()) {
        while (query.next()) {
            SensorRepository sensorRepository;
            auto sensor = sensorRepository.getSensorById(query.value("sensor_id").toLongLong());
            if(!sensor) {
                return {};
            }

            measurements.append(Measurement(query.value("id").toLongLong(),
                                            query.value("data").toByteArray(),
                                            query.value("recorded_at").toDateTime(),
                                            sensor.value()));
        }
    } else {
        qDebug() << "Database error while fetching Measurements by Sensor and Date:" << query.lastError().text();
    }

    return measurements;
}

std::optional<Measurement> MeasurementRepository::createMeasurement(const QByteArray& data, qint64 sensorId) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare(R"(
        INSERT INTO measurement (data, sensor_id)
        VALUES (:data, :sensor_id)
        RETURNING id, data, recorded_at, sensor_id
    )");

    query.bindValue(":data", data);
    query.bindValue(":sensor_id", sensorId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Database error while creating measurement:" << query.lastError().text();
        return std::nullopt;
    }
    SensorRepository sensorRepository;
    qint64 id = query.value("id").toLongLong();
    QByteArray storedData = query.value("data").toByteArray();
    QDateTime recordedAt = query.value("recorded_at").toDateTime();
    auto retrievedSensor = sensorRepository.getSensorById(query.value("sensor_id").toLongLong());
    if(!retrievedSensor) {
        return std::nullopt;
    }
    return Measurement(id, storedData, recordedAt, retrievedSensor.value());
}

