#ifndef RESPONSEFACTORYH_H
#define RESPONSEFACTORYH_H

#include <QtHttpServer/QHttpServerResponse>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString> // Required for QString

class ResponseFactory
{
public:
    static QHttpServerResponse createResponse(const QString &content, QHttpServerResponse::StatusCode statusCode);

    static QHttpServerResponse createJsonResponse(const QByteArray &content, QHttpServerResponse::StatusCode statusCode);

    // New method for creating standardized JSON error responses
    static QHttpServerResponse createErrorResponse(const QString &errorMessage, QHttpServerResponse::StatusCode statusCode);

    static void addCorsHeaders(QHttpServerResponse &response);
private:

};

#endif // RESPONSEFACTORYH_H
