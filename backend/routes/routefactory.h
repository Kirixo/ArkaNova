#ifndef ROUTEFACTORY_H
#define ROUTEFACTORY_H
#include <QtHttpServer/QHttpServer>
#include "../controllers/dbcontroller.h"


class RouteFactory
{
public:
    explicit RouteFactory(std::shared_ptr<QHttpServer> server, std::shared_ptr<DBController> dbcontroller);

    void registerAllRoutes();

    void setupBackupRoutes();
private:
    std::shared_ptr<DBController> dbcontroller_;
    std::shared_ptr<QHttpServer> server_;

    void setupUserRoutes();
    void setupSensorRoutes();
    void setupSolarPanelRoutes();
    void setupMeasurementRoutes();

    void handleOptionsRequest();

};



#endif // ROUTEFACTORY_H
