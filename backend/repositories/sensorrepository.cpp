#include "sensorrepository.h"
#include "solarpanelrepository.h"
#include "../controllers/dbcontroller.h"
#include <qsqlerror.h>

std::optional<Sensor> SensorRepository::getSensorById(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare("SELECT id, solar_panel_id, sensor_type_id AS type FROM sensor WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        SensorTypeRepository sensorTypeRepository;
        SolarPanelRepository solarPanelRepository;

        auto solarPanelId = solarPanelRepository.fetchById(query.value("solar_panel_id").toInt());
        auto sensorTypeId = sensorTypeRepository.fetchById(query.value("type").toInt());
        if (!solarPanelId || !sensorTypeId) {
            return std::nullopt;
        }

        return Sensor(query.value("id").toLongLong(),
                      solarPanelId.value(),sensorTypeId.value());

    }
    return std::nullopt;
}

QList<Sensor> SensorRepository::getSensorsByPanelId() {
    QList<Sensor> sensors;
    QSqlQuery query(DBController::getDatabase());
    query.prepare("SELECT id, solar_panel_id, sensor_type_id AS type FROM sensor");

    if (query.exec()) {
        SensorTypeRepository sensorTypeRepository;
        SolarPanelRepository solarPanelRepository;
        while (query.next()) {

            auto solarPanelId = solarPanelRepository.fetchById(query.value("solar_panel_id").toInt());
            auto sensorTypeId = sensorTypeRepository.fetchById(query.value("type").toInt());
            if (!solarPanelId || !sensorTypeId) {
                return {};
            }
            sensors.append(Sensor(query.value("id").toLongLong(),
                                  solarPanelId.value(),
                                  sensorTypeId.value()));
        }
    }
    return sensors;
}

bool SensorRepository::deleteSensor(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare("DELETE FROM sensor WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() > 0;
}

std::optional<Sensor> SensorRepository::createSensor(const Sensor& sensor) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare(R"(INSERT INTO sensor (solar_panel_id, sensor_type_id)
                    VALUES (:solar_panel_id, :type_id);)");
    query.bindValue(":solar_panel_id", sensor.solarPanel().id());
    query.bindValue(":type_id", sensor.type().id());

    if (query.exec()) {
            auto sensorId = query.lastInsertId().toLongLong();

            return Sensor(sensorId, sensor.solarPanel(), sensor.type());
    }

    qDebug() << "Database error while creating Sensor:" << query.lastError().text();
    return std::nullopt;
}
