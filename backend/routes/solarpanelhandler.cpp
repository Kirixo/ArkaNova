#include "solarpanelhandler.h"
#include <qhttpserverrequest.h>

SolarPanelHandler::SolarPanelHandler()
    : solarPanelRepository_(std::make_shared<SolarPanelRepository>()) {}

QHttpServerResponse SolarPanelHandler::getSolarPanel(const QHttpServerRequest& request) {
    bool ok;
    qint64 panelId = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Solar panel id is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto panel = solarPanelRepository_->fetchById(panelId);
    if (panel) {
        QByteArray responseData = QJsonDocument(panel->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createResponse("Solar panel not found.", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse SolarPanelHandler::getSolarPanelListByUser(const QHttpServerRequest& request) {
    int userId = request.query().queryItemValue("user_id").toInt();
    int page = request.query().queryItemValue("page").toInt() ?: 1;
    int limit = request.query().queryItemValue("limit").toInt() ?: 25;

    auto panels = solarPanelRepository_->getPanelsByUser(userId, page, limit);
    QJsonArray panelArray;
    for (const auto& panel : panels) {
        panelArray.append(panel.toJson());
    }
    QJsonObject response;
    response["panels"] = panelArray;
    response["total_count"] = 100;
    return ResponseFactory::createJsonResponse(QJsonDocument(response).toJson(), QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse SolarPanelHandler::createSolarPanel(const QHttpServerRequest& request) {
    QJsonParseError err;
    auto json = QJsonDocument::fromJson(request.body(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("location") || !json.contains("user_id")) {
        return ResponseFactory::createResponse("Missing required fields (location, user_id).",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }


    SolarPanel panel(-1, json.value("location").toString(), User(json.value("user_id").toInt()));
    if (auto newPanel = solarPanelRepository_->createSolarPanel(panel); newPanel) {
        QByteArray responseData = QJsonDocument(newPanel->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Created);
    }

    return ResponseFactory::createResponse("Failed to create solar panel.", QHttpServerResponse::StatusCode::InternalServerError);
}

QHttpServerResponse SolarPanelHandler::updateSolarPanel(const QHttpServerRequest& request) {
    bool ok;
    qint64 id = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Solar panel id is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonParseError err;
    auto json = QJsonDocument::fromJson(request.body(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    auto panel = solarPanelRepository_->fetchById(id);
    if (!panel) {
        return ResponseFactory::createResponse("Solar panel not found.", QHttpServerResponse::StatusCode::NotFound);
    }

    panel->setLocation(json.value("location").toString());
    if (solarPanelRepository_->updateSolarPanel(*panel)) {
        QByteArray responseData = QJsonDocument(panel->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }

    return ResponseFactory::createResponse("Failed to update solar panel.", QHttpServerResponse::StatusCode::InternalServerError);
}

QHttpServerResponse SolarPanelHandler::deleteSolarPanel(const QHttpServerRequest& request) {
    bool ok;
    qint64 panelId = request.query().queryItemValue("id").toLongLong(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("Solar panel id is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    if (solarPanelRepository_->deleteSolarPanel(panelId)) {
        return ResponseFactory::createResponse("Solar panel deleted successfully.",
                                               QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createResponse("Solar panel not found.", QHttpServerResponse::StatusCode::NotFound);
}

