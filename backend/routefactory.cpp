#include "routefactory.h"

RouteFactory::RouteFactory(std::shared_ptr<QHttpServer> server, std::shared_ptr<DBController> dbcontroller)
    : server_(server), dbcontroller_(dbcontroller) {}

void RouteFactory::registerAllRoutes()
{
    handleOptionsRequest();
    setupUserRoutes();
}

void RouteFactory::setupUserRoutes() {
    // server_->route("/api/users", QHttpServerRequest::Method::Get, UserHandler::getUser);
    // server_->route("/api/users", QHttpServerRequest::Method::Patch, UserHandler::updateUser);
    // server_->route("/api/users/register", QHttpServerRequest::Method::Post, UserHandler::registerUser);
    // server_->route("/api/users/login", QHttpServerRequest::Method::Post, UserHandler::loginUser);
    // server_->route("/api/users/list", QHttpServerRequest::Method::Get, UserHandler::getUserList);
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
