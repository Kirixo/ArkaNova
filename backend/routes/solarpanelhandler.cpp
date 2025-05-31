#include "solarpanelhandler.h"
#include "../utils/responsefactory.h" // Assuming ResponseFactory is in this path or similar
#include "../models/user.h"    // Assuming User model is in this path
#include <qhttpserverrequest.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDateTime> // Required for QDateTime()

// Make sure SolarPanelRepository is included, usually via solarpanelhandler.h -> solarpanelrepository.h

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
    bool okUserId;
    int userId = request.query().queryItemValue("user_id").toInt(&okUserId);

    if (!okUserId) { // It's good practice to check conversion success for all parameters
        return ResponseFactory::createResponse("User ID is missing or invalid.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    bool okPage;
    int page = request.query().queryItemValue("page").toInt(&okPage);
    if (!okPage || page <= 0) page = 1; // Default to 1 if missing, invalid, or non-positive

    bool okLimit;
    int limit = request.query().queryItemValue("limit").toInt(&okLimit);
    if (!okLimit || limit <= 0) limit = 25; // Default to 25 if missing, invalid, or non-positive


    auto panels = solarPanelRepository_->getPanelsByUser(userId, page, limit);
    QJsonArray panelArray;
    for (const auto& panel : panels) {
        panelArray.append(panel.toJson());
    }
    QJsonObject response;
    response["panels"] = panelArray;
    // TODO: Replace hardcoded total_count with actual count from repository for proper pagination
    response["total_count"] = 100; // This is hardcoded
    return ResponseFactory::createJsonResponse(QJsonDocument(response).toJson(), QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse SolarPanelHandler::createSolarPanel(const QHttpServerRequest& request) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &err);

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format: " + err.errorString(),
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    if (!doc.isObject()) {
        return ResponseFactory::createResponse("Invalid JSON format: Expected a JSON object.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonObject json = doc.object();


    if (!json.contains("location") || !json.contains("user_id")) {
        return ResponseFactory::createResponse("Missing required fields (location, user_id).",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    // Ensure user_id is an integer
    if (!json.value("user_id").isDouble()) { // QJson uses doubles for numbers
        return ResponseFactory::createResponse("Field 'user_id' must be an integer.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    int userId = json.value("user_id").toInt();


    // Corrected SolarPanel constructor call:
    // Pass default QDateTime() for createdAt and updatedAt, as these will be set by the database.
    // The User object is constructed with just an ID here. Ensure User class supports this.
    SolarPanel panelToCreate(-1,
                             json.value("location").toString(),
                             User(userId), // Assuming User(int id) constructor exists
                             QDateTime(),  // Default QDateTime for createdAt
                             QDateTime()); // Default QDateTime for updatedAt

    if (auto newPanel = solarPanelRepository_->createSolarPanel(panelToCreate); newPanel) {
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
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &err);

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format: " + err.errorString(),
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    if (!doc.isObject()) {
        return ResponseFactory::createResponse("Invalid JSON format: Expected a JSON object.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonObject json = doc.object();


    auto panelOptional = solarPanelRepository_->fetchById(id);
    if (!panelOptional) {
        return ResponseFactory::createResponse("Solar panel not found.", QHttpServerResponse::StatusCode::NotFound);
    }

    SolarPanel panel = panelOptional.value(); // Get the actual panel object

    // Update fields if present in JSON
    if (json.contains("location") && json.value("location").isString()) {
        panel.setLocation(json.value("location").toString());
    }
    // Add user_id update if needed, e.g.:
    // if (json.contains("user_id") && json.value("user_id").isDouble()) {
    //     panel.setUser(User(json.value("user_id").toInt()));
    // }


    if (solarPanelRepository_->updateSolarPanel(panel)) {
        // For the response to contain the *very latest* updated_at timestamp from the DB,
        // solarPanelRepository_->updateSolarPanel would need to return the updated SolarPanel object,
        // or we would need to call fetchById(id) again here.
        // Currently, panel.toJson() will use the panel's state *before* the DB's `updated_at` trigger might modify it
        // if `updateSolarPanel` doesn't refresh the object. However, the SQL update already sets CURRENT_TIMESTAMP.
        // The panel object's updatedAt_ field will reflect the value it had when fetched, or if set manually.
        // The previous changes ensure SolarPanelRepository::updateSolarPanel uses CURRENT_TIMESTAMP in SQL,
        // and the returned JSON will reflect the SolarPanel object state.
        // To get the absolute latest from DB after update trigger, an explicit fetch would be needed.
        // For now, we return the state of the 'panel' object as modified locally.
        // A better approach if strict latest data is needed in response:
        auto updatedPanel = solarPanelRepository_->fetchById(id); // Re-fetch to get DB-updated timestamps
        if (updatedPanel) {
            QByteArray responseData = QJsonDocument(updatedPanel->toJson()).toJson(QJsonDocument::Compact);
            return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
        }
        // Fallback or error if re-fetch fails, though unlikely if update succeeded.
        return ResponseFactory::createResponse("Failed to retrieve updated solar panel.", QHttpServerResponse::StatusCode::InternalServerError);

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
        // No JSON body needed for a simple success message on delete
        return ResponseFactory::createResponse("Solar panel deleted successfully.",
                                               QHttpServerResponse::StatusCode::Ok);
    }
    // The repository returns false if delete failed (e.g. 0 rows affected)
    // This could mean "not found" or some other DB constraint/issue
    return ResponseFactory::createResponse("Failed to delete solar panel. It might not exist or an error occurred.", QHttpServerResponse::StatusCode::NotFound); // Or InternalServerError depending on expectation
}

// Note: The extra closing brace "}" at the end of the original file was removed.
