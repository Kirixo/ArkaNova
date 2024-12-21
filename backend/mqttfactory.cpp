#include "mqttfactory.h"

MqttFactory::MqttFactory(const QString &broker, int port, const QString &username, const QString &password, QObject *parent)
    : QObject(parent), broker_(broker), port_(port), username_(username), password_(password)
{
    mqttClient_ = new QMqttClient(this);
    mqttClient_->setHostname(broker_);
    mqttClient_->setPort(port_);
    mqttClient_->setUsername(username_);
    mqttClient_->setPassword(password_);

    connect(mqttClient_, &QMqttClient::stateChanged, this, &MqttFactory::handleStateChange);
    connect(mqttClient_, &QMqttClient::errorChanged, this, &MqttFactory::handleError);
    connect(mqttClient_, &QMqttClient::messageReceived, this, &MqttFactory::handleMessage);
}

MqttFactory::~MqttFactory()
{
    mqttClient_->disconnectFromHost();
}

void MqttFactory::setupMqttConnections()
{
    mqttClient_->connectToHost();
    if (mqttClient_->state() != QMqttClient::Connected) {
        Logger::instance().log("MQTT: Connection to broker initiated", Logger::LogLevel::Info);
    }
}

void MqttFactory::subscribeToTopic(const QString &topic)
{
    if (mqttClient_->state() == QMqttClient::Connected) {
        auto subscription = mqttClient_->subscribe(topic);
        if (subscription) {
            Logger::instance().log("Subscribed to topic: " + topic, Logger::LogLevel::Info);
        } else {
            Logger::instance().log("Failed to subscribe to topic: " + topic, Logger::LogLevel::Error);
        }
    } else {
        Logger::instance().log("Cannot subscribe, MQTT client is not connected.", Logger::LogLevel::Error);
    }
}

void MqttFactory::disconnectFromBroker()
{
    mqttClient_->disconnectFromHost();
    Logger::instance().log("MQTT: Disconnected from broker.", Logger::LogLevel::Info);
}

void MqttFactory::setupAllTopics()
{
    subscribeToTopic("default/topic");
}

void MqttFactory::handleStateChange(QMqttClient::ClientState state)
{
    switch (state) {
    case QMqttClient::Disconnected:
        Logger::instance().log("MQTT: Disconnected from broker.", Logger::LogLevel::Info);
        break;
    case QMqttClient::Connecting:
        Logger::instance().log("MQTT: Connecting to broker...", Logger::LogLevel::Info);
        break;
    case QMqttClient::Connected:
        setupAllTopics();
        Logger::instance().log("MQTT: Connected to broker successfully.", Logger::LogLevel::Info);
        break;
    }
}

void MqttFactory::handleError(QMqttClient::ClientError error)
{
    Logger::instance().log("MQTT: Error occurred: " + QString::number(error), Logger::LogLevel::Error);
}

void MqttFactory::handleMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    Logger::instance().log(QString("MQTT: Message received. Topic = %1, Message = %2")
                               .arg(topic.name(), QString(message)),
                               Logger::LogLevel::Info);

    emit messageReceived(topic.name(), message);
}
