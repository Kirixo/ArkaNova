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
    UserHandler userHandler;
    server_->route("/api/users", QHttpServerRequest::Method::Get,
                   [&userHandler](const QHttpServerRequest& request) {
                        return userHandler.getUser(request);
                   });
    server_->route("/api/users", QHttpServerRequest::Method::Patch,
                   [&userHandler](const QHttpServerRequest& request){
                        return userHandler.updateUser(request);
                   });
    server_->route("/api/users", QHttpServerRequest::Method::Delete,
                   [&userHandler](const QHttpServerRequest& request){
                       return userHandler.deleteUser(request);
                   });
    server_->route("/api/users/register", QHttpServerRequest::Method::Post,
                   [&userHandler](const QHttpServerRequest& request){
                       return userHandler.registerUser(request);
                   });
    server_->route("/api/users/login", QHttpServerRequest::Method::Post,
                   [&userHandler](const QHttpServerRequest& request){
                       return userHandler.loginUser(request);
                   });
}

void RouteFactory::setupSensorRoutes() {
    SensorHandler sensorHandler;
    server_->route("/api/sensor", QHttpServerRequest::Method::Get,
                   [&sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler.getSensor(request);
                   });
    server_->route("/api/sensor/list/solarpanel", QHttpServerRequest::Method::Get,
                   [&sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler.getSensorList(request);
                   });
    server_->route("/api/sensor", QHttpServerRequest::Method::Delete,
                   [&sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler.deleteSensor(request);
                   });
    server_->route("/api/sensor", QHttpServerRequest::Method::Post,
                   [&sensorHandler](const QHttpServerRequest& request) {
                       return sensorHandler.createSensor(request);
                   });
}

void RouteFactory::setupSolarPanelRoutes() {
    SolarPanelHandler solarPanelHandler;

    server_->route("/api/solarpanel", QHttpServerRequest::Method::Get,
                   [&solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler.getSolarPanel(request);
                   });

    server_->route("/api/solarpanel/list/user", QHttpServerRequest::Method::Get,
                   [&solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler.getSolarPanelListByUser(request);
                   });

    server_->route("/api/solarpanel", QHttpServerRequest::Method::Patch,
                   [&solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler.updateSolarPanel(request);
                   });

    server_->route("/api/solarpanel", QHttpServerRequest::Method::Delete,
                   [&solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler.deleteSolarPanel(request);
                   });

    server_->route("/api/solarpanel", QHttpServerRequest::Method::Post,
                   [&solarPanelHandler](const QHttpServerRequest& request) {
                       return solarPanelHandler.createSolarPanel(request);
                   });}

void RouteFactory::setupMeasurementRoutes() {
    MeasurementHandler measurementHandler;
    server_->route("/api/measurement", QHttpServerRequest::Method::Get,
                   [&measurementHandler](const QHttpServerRequest& request) {
                       return measurementHandler.getMeasurementById(request);
                   });

    server_->route("/api/measurement/list/sensor", QHttpServerRequest::Method::Get,
                   [&measurementHandler](const QHttpServerRequest& request) {
                       return measurementHandler.getMeasurementsBySensor(request);
                   });}

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
