#include "userhandler.h"
#include "../utils/responsefactory.h" // Ensure this path is correct
#include "../models/user.h"           // Ensure this path is correct
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>     // Required for QJsonArray
#include <QJsonParseError>
#include <QDebug>
#include <QUrlQuery>      // Required for QUrlQuery

// Constructor
UserHandler::UserHandler() : userRepository_(std::make_shared<UserRepository>()) {}

// Get a single user by ID
QHttpServerResponse UserHandler::getUser(const QHttpServerRequest& request) {
    bool ok;
    // Corrected: Use QUrlQuery to parse query parameters
    QUrlQuery queryParams(request.url().query());
    qint64 userId = queryParams.queryItemValue("id").toLongLong(&ok);

    if (!ok || userId <= 0) {
        return ResponseFactory::createErrorResponse("User ID is missing or invalid.",
                                                    QHttpServerResponse::StatusCode::BadRequest);
    }

    auto userOptional = userRepository_->getUserById(userId);
    if (userOptional) {
        QByteArray responseData = QJsonDocument(userOptional->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createErrorResponse("User not found.", QHttpServerResponse::StatusCode::NotFound);
}

// Update an existing user
QHttpServerResponse UserHandler::updateUser(const QHttpServerRequest& request) {
    bool ok;
    // Corrected: Use QUrlQuery to parse query parameters
    QUrlQuery queryParams(request.url().query());
    qint64 userId = queryParams.queryItemValue("id").toLongLong(&ok);

    if (!ok || userId <= 0) {
        return ResponseFactory::createErrorResponse("User ID is missing or invalid.",
                                                    QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        return ResponseFactory::createErrorResponse("Invalid JSON format: " + parseError.errorString(), QHttpServerResponse::StatusCode::BadRequest);
    }
    if (!doc.isObject()){
        return ResponseFactory::createErrorResponse("Invalid JSON format: Expected a JSON object.", QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonObject json = doc.object();

    auto userOptional = userRepository_->findUserById(userId);
    if (!userOptional) {
        return ResponseFactory::createErrorResponse("User not found.", QHttpServerResponse::StatusCode::NotFound);
    }

    User user = userOptional.value();
    bool changed = false;
    if (json.contains("email") && json.value("email").isString()) {
        user.setEmail(json.value("email").toString());
        changed = true;
    }
    // Password update should involve hashing and be handled carefully.
    // For simplicity, direct password update from JSON is omitted here.
    // If you need to update password, ensure it's hashed.
    // If you want to allow password updates here, you'd add:
    // if (json.contains("password") && json.value("password").isString()) {
    //     user.setPassword(json.value("password").toString()); // Remember to hash this in repository if needed
    //     changed = true;
    // }


    if (!changed) {
        return ResponseFactory::createErrorResponse("No updatable fields provided or no changes detected (e.g., email).",
                                                    QHttpServerResponse::StatusCode::BadRequest);
    }

    if (userRepository_->updateUser(user)) {
        auto updatedUserOptional = userRepository_->findUserById(userId);
        if (updatedUserOptional) {
            QByteArray responseData = QJsonDocument(updatedUserOptional->toJson()).toJson(QJsonDocument::Compact);
            return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
        } else {
            return ResponseFactory::createErrorResponse("User updated but could not be retrieved.", QHttpServerResponse::StatusCode::InternalServerError);
        }
    }
    return ResponseFactory::createErrorResponse("Failed to update user.", QHttpServerResponse::StatusCode::InternalServerError);
}

// Delete a user
QHttpServerResponse UserHandler::deleteUser(const QHttpServerRequest& request) {
    bool ok;
    // Corrected: Use QUrlQuery to parse query parameters
    QUrlQuery queryParams(request.url().query());
    qint64 userId = queryParams.queryItemValue("id").toLongLong(&ok);

    if (!ok || userId <= 0) {
        return ResponseFactory::createErrorResponse("User ID is missing or invalid.",
                                                    QHttpServerResponse::StatusCode::BadRequest);
    }

    if (userRepository_->deleteUser(userId)) {
        QJsonObject responseObject;
        responseObject["message"] = "User deleted successfully.";
        QByteArray responseData = QJsonDocument(responseObject).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createErrorResponse("Failed to delete user. User might not exist or an error occurred.",
                                                QHttpServerResponse::StatusCode::NotFound);
}

// Register a new user
QHttpServerResponse UserHandler::registerUser(const QHttpServerRequest& request) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &err);

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createErrorResponse("Invalid JSON format: " + err.errorString(), QHttpServerResponse::StatusCode::BadRequest);
    }
    if (!doc.isObject()){
        return ResponseFactory::createErrorResponse("Invalid JSON format: Expected a JSON object.", QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonObject json = doc.object();

    if (!json.contains("email") || !json.value("email").isString() ||
        !json.contains("password") || !json.value("password").isString()) {
        return ResponseFactory::createErrorResponse("Missing or invalid email or password.", QHttpServerResponse::StatusCode::BadRequest);
    }

    QString email = json.value("email").toString();
    QString password = json.value("password").toString();

    if (userRepository_->findUserByEmail(email)) {
        return ResponseFactory::createErrorResponse("Email already in use.",
                                                    QHttpServerResponse::StatusCode::Conflict);
    }

    User userToCreate(-1, email, password); // ID will be set by DB

    auto createdUserOptional = userRepository_->createUser(userToCreate);
    if (createdUserOptional) {
        QByteArray responseData = QJsonDocument(createdUserOptional->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Created);
    }
    return ResponseFactory::createErrorResponse("Failed to create user. Email might have been taken or a database error occurred.",
                                                QHttpServerResponse::StatusCode::InternalServerError);
}

// Login a user
QHttpServerResponse UserHandler::loginUser(const QHttpServerRequest& request) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &err);

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createErrorResponse("Invalid JSON format: " + err.errorString(), QHttpServerResponse::StatusCode::BadRequest);
    }
    if (!doc.isObject()){
        return ResponseFactory::createErrorResponse("Invalid JSON format: Expected a JSON object.", QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonObject json = doc.object();

    if (!json.contains("email") || !json.value("email").isString() ||
        !json.contains("password") || !json.value("password").isString()) {
        return ResponseFactory::createErrorResponse("Missing or invalid email or password.", QHttpServerResponse::StatusCode::BadRequest);
    }

    QString email = json.value("email").toString();
    QString plainPassword = json.value("password").toString();

    if (userRepository_->verifyPassword(email, plainPassword)) {
        auto userOptional = userRepository_->findUserByEmail(email); // Fetch user details again after verification
        if (userOptional) {
            QJsonObject fullResponse = userOptional->toJson();
            // Example: Add JWT token here
            // fullResponse["token"] = generateJwtToken(userOptional->id());
            return ResponseFactory::createJsonResponse(QJsonDocument(fullResponse).toJson(QJsonDocument::Compact),
                                                       QHttpServerResponse::StatusCode::Ok);
        }
    }
    return ResponseFactory::createErrorResponse("Email or password is not correct.", QHttpServerResponse::StatusCode::Unauthorized);
}

// New method implementation for listing users
QHttpServerResponse UserHandler::getUserList(const QHttpServerRequest& request) {
    QUrlQuery queryParams(request.url().query()); // This was already correct
    bool pageOk, limitOk;
    int page = queryParams.queryItemValue("page").toInt(&pageOk);
    int limit = queryParams.queryItemValue("limit").toInt(&limitOk);

    if (!pageOk || page <= 0) {
        page = 1; // Default page
    }
    if (!limitOk || limit <= 0) {
        limit = 25; // Default limit
    }
    if (limit > 100) { // Optional: Max limit
        limit = 100;
    }

    QList<User> users = userRepository_->getUsers(page, limit);
    int totalCount = userRepository_->getTotalUserCount();

    QJsonArray usersArray;
    for (const User& user : users) {
        usersArray.append(user.toJson());
    }

    QJsonObject responseObject;
    responseObject["users"] = usersArray;
    responseObject["total_count"] = totalCount;
    responseObject["page"] = page;
    responseObject["limit"] = limit;
    responseObject["total_pages"] = (totalCount + limit - 1) / limit; // Calculate total pages

    return ResponseFactory::createJsonResponse(QJsonDocument(responseObject).toJson(QJsonDocument::Compact),
                                               QHttpServerResponse::StatusCode::Ok);
}
