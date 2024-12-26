#include "routefactory.h"
#include "responsefactory.h"

RouteFactory::RouteFactory(std::shared_ptr<QHttpServer> server, std::shared_ptr<DBController> dbcontroller)
    : server_(server), dbcontroller_(dbcontroller) {}

void RouteFactory::registerAllRoutes()
{
    handleOptionsRequest();
    setupUserRoutes();
    setupSensorRoutes();
    setupSolarPanelRoutes();
    setupMeasurementRoutes();
}

void RouteFactory::setupUserRoutes() {
    server_->route("/api/users", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/users", QHttpServerRequest::Method::Patch,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/users", QHttpServerRequest::Method::Delete,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/users/register", QHttpServerRequest::Method::Post,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/users/login", QHttpServerRequest::Method::Post,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
}

void RouteFactory::setupSensorRoutes() {
    server_->route("/api/sensor", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                   QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/sensor/list/solarpanel", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/sensor", QHttpServerRequest::Method::Delete,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/sensor", QHttpServerRequest::Method::Post,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                    QHttpServerResponse::StatusCode::InternalServerError); });
}

void RouteFactory::setupSolarPanelRoutes() {
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/solarpanel/list/user", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Patch,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Delete,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Post,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
}

void RouteFactory::setupMeasurementRoutes() {
    server_->route("/api/measurement", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
    server_->route("/api/measurement/list/sensor", QHttpServerRequest::Method::Get,
                   [this](){return ResponseFactory::createJsonResponse("Hasn't been realized.",
                                                                         QHttpServerResponse::StatusCode::InternalServerError); });
}

void RouteFactory::handleOptionsRequest()
{
    server_->route("/*", QHttpServerRequest::Method::Options, [this](const QHttpServerRequest &request) {
        QHttpServerResponse response(
            QByteArray(),
            QHttpServerResponse::StatusCode::NoContent
            );

        response.setHeader("Content-Type", "text/plain");
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

        return response;
    });
}
