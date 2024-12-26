#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <qhttpserverrequest.h>
#include <qhttpserverresponse.h>
#include "../repositories/sensorrepository.h"
#include "../repositories/solarpanelrepository.h"
class SensorHandler {
public:
    SensorHandler();

    QHttpServerResponse getSensor(const QHttpServerRequest& request);
    QHttpServerResponse getSensorList(const QHttpServerRequest& request);
    QHttpServerResponse deleteSensor(const QHttpServerRequest& request);
    QHttpServerResponse createSensor(const QHttpServerRequest& request);

private:
    std::shared_ptr<SensorRepository> sensorRepository_;
};


#endif // SENSORHANDLER_H
