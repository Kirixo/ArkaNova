#include "mqttmeasurementhandler.h"
#include "../repositories/measurementrepository.h"
#include "../utils/logger.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include "../repositories/sensorrepository.h"


MqttMeasurementHandler::MqttMeasurementHandler() {}


void MqttMeasurementHandler::saveMeasurementToDatabase(const QByteArray& message)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        Logger::instance().log("MQTT: Failed to parse JSON message: " + parseError.errorString(), Logger::LogLevel::Error);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("data") || !jsonObj.contains("sensor_id")) {
        Logger::instance().log("MQTT: JSON message missing required fields (data, sensor_id).", Logger::LogLevel::Error);
        return;
    }
    SensorRepository sensorRepository;
    QByteArray data = QByteArray::number(jsonObj.value("data").toDouble());
    qDebug() << data;
    auto sensor = sensorRepository.getSensorById(jsonObj.value("sensor_id").toVariant().toLongLong());

    if(!sensor) {
        return;
    }

    Measurement measurement (-1, data, QDateTime(), sensor.value());
    if (measurementRepository_.createMeasurement(measurement.data(), sensor->id())) {
        Logger::instance().log("MQTT: Measurement saved successfully.", Logger::LogLevel::Info);
    } else {
        Logger::instance().log("MQTT: Failed to save measurement to database.", Logger::LogLevel::Error);
    }
}
