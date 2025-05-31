#include "responsefactory.h"
#include <QJsonObject> // Required for QJsonObject
#include <QJsonDocument> // Required for QJsonDocument

QHttpServerResponse ResponseFactory::createResponse(const QString &content, QHttpServerResponse::StatusCode statusCode)
{
    QHttpServerResponse response(content.toUtf8(), statusCode); // Direct initialization
    addCorsHeaders(response);
    response.setHeader("Content-Type", "text/plain; charset=utf-8"); // Specify charset
    return response;
}

QHttpServerResponse ResponseFactory::createJsonResponse(const QByteArray &content, QHttpServerResponse::StatusCode statusCode)
{
    QHttpServerResponse response(content, statusCode); // Direct initialization
    addCorsHeaders(response);
    response.setHeader("Content-Type", "application/json; charset=utf-8"); // Specify charset
    return response;
}

// Implementation of the new method
QHttpServerResponse ResponseFactory::createErrorResponse(const QString &errorMessage, QHttpServerResponse::StatusCode statusCode)
{
    QJsonObject errorObject;
    errorObject["error"] = errorMessage; // Create a JSON object with an "error" key

    // Convert the JSON object to QByteArray
    QByteArray jsonData = QJsonDocument(errorObject).toJson(QJsonDocument::Compact);

    // Use the existing createJsonResponse method to send it
    return createJsonResponse(jsonData, statusCode);
}

void ResponseFactory::addCorsHeaders(QHttpServerResponse &response)
{
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With"); // Added X-Requested-With as it's common
    response.setHeader("Access-Control-Max-Age", "86400"); // Optional: How long the results of a preflight request can be cached
}
