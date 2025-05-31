#include "measurementhandler.h"
#include <qjsonobject.h>
#include "../utils/responsefactory.h"
#include <QJsonArray>

MeasurementHandler::MeasurementHandler() {
    // It's good practice to initialize shared_ptr in the constructor
    measurementRepository_ = std::make_shared<MeasurementRepository>();
}

QHttpServerResponse MeasurementHandler::getMeasurementById(const QHttpServerRequest& request) {
    bool ok;
    qint64 measurementId = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Measurement ID is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto measurement = measurementRepository_->fetchById(measurementId);
    if (measurement) {
        QByteArray responseData = QJsonDocument(measurement->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createResponse("Measurement not found.", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse MeasurementHandler::getMeasurementsBySensor(const QHttpServerRequest& request) {
    bool ok;
    qint64 sensorId = request.query().queryItemValue("sensor_id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Sensor ID is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto startDateStr = request.query().queryItemValue("start_date");
    auto endDateStr = request.query().queryItemValue("end_date");

    QDateTime startDate = startDateStr.isEmpty()
                              ? QDateTime()
                              : QDateTime::fromString(startDateStr, Qt::ISODate);
    QDateTime endDate = endDateStr.isEmpty() ? QDateTime::currentDateTime()
                                             : QDateTime::fromString(endDateStr, Qt::ISODate);

    auto measurements = measurementRepository_->getMeasurementsBySensorAndDate(sensorId, startDate, endDate);
    QJsonArray jsonMeasurements;
    for (const auto& measurement : measurements) {
        jsonMeasurements.append(measurement.toJson());
    }
    QJsonObject response;
    response["measurements"] = jsonMeasurements;
    // The total_count seems to be hardcoded, you might want to adjust this
    response["total_count"] = measurements.count(); // Or actual count if paginating

    return ResponseFactory::createJsonResponse(QJsonDocument(response).toJson(),
                                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse MeasurementHandler::getLatestMeasurementBySensor(const QHttpServerRequest& request) {
    bool ok;
    qint64 sensorId = request.query().queryItemValue("sensor_id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Sensor ID is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto measurement = measurementRepository_->getLatestMeasurementBySensorId(sensorId);
    if (measurement) {
        QByteArray responseData = QJsonDocument(measurement->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createResponse("Latest measurement not found for this sensor or sensor does not exist.", QHttpServerResponse::StatusCode::NotFound);
}
