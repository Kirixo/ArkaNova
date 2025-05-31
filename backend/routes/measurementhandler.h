#ifndef MEASUREMENTHANDLER_H
#define MEASUREMENTHANDLER_H
#include <qhttpserverresponse.h>
#include <qhttpserverrequest.h>
#include "../repositories/measurementrepository.h"

class MeasurementHandler
{
public:
    MeasurementHandler();
    QHttpServerResponse getMeasurementsBySensor(const QHttpServerRequest &request);
    QHttpServerResponse getMeasurementById(const QHttpServerRequest &request);
    QHttpServerResponse getLatestMeasurementBySensor(const QHttpServerRequest& request); // New method
private:
    std::shared_ptr<MeasurementRepository> measurementRepository_;
};

#endif // MEASUREMENTHANDLER_H
