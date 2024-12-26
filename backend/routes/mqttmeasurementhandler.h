#ifndef MQTTMEASUREMENTHANDLER_H
#define MQTTMEASUREMENTHANDLER_H
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qmqtttopicname.h>
#include "../repositories/measurementrepository.h"

class MqttMeasurementHandler
{
public:
    MqttMeasurementHandler();
    void saveMeasurementToDatabase(const QByteArray &message);
    // void handleMessage(const QByteArray &message, const QMqttTopicName &topic);
private:
    MeasurementRepository measurementRepository_;
};

#endif // MQTTMEASUREMENTHANDLER_H
