#include "routefactory.h"
#include "../utils/responsefactory.h"
#include "../repositories/userrepository.h"
#include "measurementhandler.h"
#include "sensorhandler.h"
#include "solarpanelhandler.h"
#include "userhandler.h"

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
    if (!server_) return; // Guard against null server_

    // UserHandler should be managed, e.g. member or created per request if stateless
    // For lambda captures, if UserHandler is on stack, it's an issue for async.
    // std::make_shared or member variable is safer.
    auto userHandler = std::make_shared<UserHandler>(); // Manage lifetime

    server_->route("/api/users/list", QHttpServerRequest::Method::Get,
                   [userHandler](const QHttpServerRequest& request) {
                       return userHandler->getUserList(request);
                   });

    server_->route("/api/users", QHttpServerRequest::Method::Get,
                   [userHandler](const QHttpServerRequest& request) { // Changed from /api/user to /api/users
                       return userHandler->getUser(request);
                   });
    server_->route("/api/users", QHttpServerRequest::Method::Patch, // Changed from /api/user
                   [userHandler](const QHttpServerRequest& request){
                       return userHandler->updateUser(request);
                   });
    server_->route("/api/users", QHttpServerRequest::Method::Delete, // Changed from /api/user
                   [userHandler](const QHttpServerRequest& request){
                       return userHandler->deleteUser(request);
                   });
    server_->route("/api/users/register", QHttpServerRequest::Method::Post,
                   [userHandler](const QHttpServerRequest& request){
                       return userHandler->registerUser(request);
                   });
    server_->route("/api/users/login", QHttpServerRequest::Method::Post,
                   [userHandler](const QHttpServerRequest& request){
                       return userHandler->loginUser(request);
                   });
}

void RouteFactory::setupSensorRoutes() {
    if (!server_) return;
    auto sensorHandler = std::make_shared<SensorHandler>();

    server_->route("/api/sensor", QHttpServerRequest::Method::Get,
                   [sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler->getSensor(request);
                   });
    server_->route("/api/sensor/list/solarpanel", QHttpServerRequest::Method::Get,
                   [sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler->getSensorList(request);
                   });
    server_->route("/api/sensor", QHttpServerRequest::Method::Delete,
                   [sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler->deleteSensor(request);
                   });
    server_->route("/api/sensor", QHttpServerRequest::Method::Post,
                   [sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler->createSensor(request);
                   });
}

void RouteFactory::setupSolarPanelRoutes() {
    if (!server_) return;
    auto solarPanelHandler = std::make_shared<SolarPanelHandler>();

    server_->route("/api/solarpanel", QHttpServerRequest::Method::Get,
                   [solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler->getSolarPanel(request);
                   });
    server_->route("/api/solarpanel/list/user", QHttpServerRequest::Method::Get,
                   [solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler->getSolarPanelListByUser(request);
                   });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Patch,
                   [solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler->updateSolarPanel(request);
                   });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Delete,
                   [solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler->deleteSolarPanel(request);
                   });
    server_->route("/api/solarpanel", QHttpServerRequest::Method::Post,
                   [solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler->createSolarPanel(request);
                   });
}

void RouteFactory::setupMeasurementRoutes() {
    if (!server_) return;
    auto measurementHandler = std::make_shared<MeasurementHandler>();

    server_->route("/api/measurement", QHttpServerRequest::Method::Get,
                   [measurementHandler](const QHttpServerRequest& request) {
                       return measurementHandler->getMeasurementById(request);
                   });
    server_->route("/api/measurement/list/sensor", QHttpServerRequest::Method::Get,
                   [measurementHandler](const QHttpServerRequest& request) {
                       return measurementHandler->getMeasurementsBySensor(request);
                   });
    server_->route("/api/measurement/latest/sensor", QHttpServerRequest::Method::Get,
                   [measurementHandler](const QHttpServerRequest& request) {
                       return measurementHandler->getLatestMeasurementBySensor(request);
                   });
}

void RouteFactory::handleOptionsRequest()
{
    server_->route("/*", QHttpServerRequest::Method::Options, [this](const QHttpServerRequest &request) {
        (void)request; // Mark as unused if request object is not used
        QHttpServerResponse response(
            QByteArray(),
            QHttpServerResponse::StatusCode::NoContent
            );

        response.setHeader("Content-Type", "text/plain");
        response.setHeader("Access-Control-Allow-Origin", "*"); // Be specific in production
        response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH"); // Added PATCH
        response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        response.setHeader("Access-Control-Max-Age", "86400");
        return response;
    });
}
