#include "sensorhandler.h"
#include "../utils/responsefactory.h"

SensorHandler::SensorHandler() : sensorRepository_(std::make_shared<SensorRepository>()) {}

QHttpServerResponse SensorHandler::getSensor(const QHttpServerRequest& request) {
    bool ok;
    qint64 sensorId = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Sensor id is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto sensor = sensorRepository_->getSensorById(sensorId);
    if (sensor) {
        QByteArray responseData = QJsonDocument(sensor->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("Sensor not found.",
                                               QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse SensorHandler::getSensorList(const QHttpServerRequest& request) {
    auto sensors = sensorRepository_->getSensorsByPanelId();
    QJsonArray sensorArray;
    for (const auto& sensor : sensors) {
        sensorArray.append(sensor.toJson());
    }
    return ResponseFactory::createJsonResponse(QJsonDocument(sensorArray).toJson(),
                                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse SensorHandler::deleteSensor(const QHttpServerRequest& request) {
    bool ok;
    qint64 sensorId = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Sensor id is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    if (sensorRepository_->deleteSensor(sensorId)) {
        return ResponseFactory::createJsonResponse("Sensor deleted successfully.",
                                                   QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("Sensor not found.",
                                               QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse SensorHandler::createSensor(const QHttpServerRequest& request) {
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(request.body(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("solar_panel_id") || !json.contains("type_id")) {
        return ResponseFactory::createResponse("Missing required fields (name or type).",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    SensorTypeRepository sensorTypeRepository;
    SolarPanelRepository solarPanelRepository;
    auto solarPanelId = solarPanelRepository.fetchById(json.value("solar_panel_id").toInt());
    auto sensorTypeId = sensorTypeRepository.fetchById(json.value("type_id").toInt());
    if (!solarPanelId || !sensorTypeId) {
        return ResponseFactory::createResponse("Sensor or solar panel hasn't been found.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    Sensor sensor(-1, solarPanelId.value(), sensorTypeId.value());
    if (auto newSensor = sensorRepository_->createSensor(sensor); newSensor) {
        QByteArray responseData = QJsonDocument(newSensor->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Created);
    }
    return ResponseFactory::createJsonResponse("Failed to create sensor.",
                                               QHttpServerResponse::StatusCode::InternalServerError);
}
