#ifndef MQTTFACTORY_H
#define MQTTFACTORY_H

#include <QObject>
#include <QMqttClient>
#include "../utils/logger.h"

class MqttFactory : public QObject
{
    Q_OBJECT

public:
    explicit MqttFactory(const QString &broker, int port, const QString &username = QString(),
                         const QString &password = QString(), QObject *parent = nullptr);
    ~MqttFactory();

    void setupMqttConnections();
    void subscribeToTopic(const QString &topic);
    void disconnectFromBroker();

    void setupAllTopics();

signals:
    void messageReceived(const QString &topic, const QByteArray &message);

private slots:
    void handleStateChange(QMqttClient::ClientState state);
    void handleError(QMqttClient::ClientError error);
    void handleMessage(const QByteArray &message, const QMqttTopicName &topic);

private:
    QMqttClient *mqttClient_;
    QString broker_;
    int port_;
    QString username_;
    QString password_;
};

#endif // MQTTFACTORY_H
