#include "userhandler.h"
#include "../utils/responsefactory.h"
#include <qhttpserverrequest.h>

UserHandler::UserHandler() : userRepository_(std::make_shared<UserRepository>()) {}

QHttpServerResponse UserHandler::getUser(const QHttpServerRequest& request) {
    bool ok;
    qint64 userId = request.query().queryItemValue("id").toInt(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("User id absence.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto user = userRepository_->getUserById(userId);
    qDebug() << user->toJson();
    if(user) {
        QByteArray responseData = QJsonDocument(user->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("Not found", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse UserHandler::updateUser(const QHttpServerRequest& request) {
    bool ok;
    qint64 userId = request.query().queryItemValue("id").toInt(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("User id absence.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    QJsonParseError parseError;
    const auto json = QJsonDocument::fromJson(request.body(), &parseError).object();

    if (parseError.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("email") && !json.contains("password")) {
        return ResponseFactory::createResponse("Missing required fields (email or password).",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }

    auto user = userRepository_->findUserById(userId);
    if (!user) {
        return ResponseFactory::createJsonResponse("User not found.", QHttpServerResponse::StatusCode::NotFound);
    }

    if (json.contains("email")) {
        user->setEmail(json.value("email").toString());
    }
    if (json.contains("password")) {
        user->setPassword(json.value("password").toString());
    }

    if (userRepository_->updateUser(user.value())) {
        return ResponseFactory::createJsonResponse("User updated successfully.", QHttpServerResponse::StatusCode::Ok);
    }

    return ResponseFactory::createJsonResponse("Failed to update user.", QHttpServerResponse::StatusCode::InternalServerError);
}


QHttpServerResponse UserHandler::deleteUser(const QHttpServerRequest& request) {
    bool ok;
    qint64 userId = request.query().queryItemValue("id").toInt(&ok);

    if (!ok) {
        return ResponseFactory::createResponse("User id absence.",
                                               QHttpServerResponse::StatusCode::BadRequest);
    }
    if (userRepository_->deleteUser(userId)) {
        return ResponseFactory::createJsonResponse("User deleted successfully.",
                                                   QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("User not found.",
                                               QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse UserHandler::registerUser(const QHttpServerRequest& request) {
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(request.body(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("email") || !json.contains("password")) {
        return ResponseFactory::createResponse("Missing email or password.", QHttpServerResponse::StatusCode::BadRequest);
    }

    QString email = json.value("email").toString();
    QString password = json.value("password").toString();
    User user(-1, email, password); // User ID is -1 as it will be auto-generated in DB
    if (userRepository_->createUser(user)) {
        QByteArray responseData = QJsonDocument(userRepository_->findUserByEmail(email)->toJson())
                                      .toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData, QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("Email already in use.",
                                               QHttpServerResponse::StatusCode::Conflict);
}

QHttpServerResponse UserHandler::loginUser(const QHttpServerRequest& request) {
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(request.body(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        return ResponseFactory::createResponse("Invalid JSON format.", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("email") || !json.contains("password")) {
        return ResponseFactory::createResponse("Missing email or password.", QHttpServerResponse::StatusCode::BadRequest);
    }

    QString email = json.value("email").toString();
    QString password = json.value("password").toString();

    auto user = userRepository_->findUserByEmail(email);
    if (user && user->id() != 0 && user->password() == password) {
        QByteArray responseData = QJsonDocument(user->toJson()).toJson(QJsonDocument::Compact);
        return ResponseFactory::createJsonResponse(responseData,
                                                   QHttpServerResponse::StatusCode::Ok);
    }
    return ResponseFactory::createJsonResponse("Email or password is not correct.", QHttpServerResponse::StatusCode::Unauthorized);
}
