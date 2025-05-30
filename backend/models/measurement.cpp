#include "measurement.h"



Measurement::Measurement() {}

Measurement::Measurement(quint64 id, QByteArray data, QDateTime recordedAt, Sensor sensor)
    : id_(id), data_(data), recordedAt_(recordedAt), sensor_(sensor)
{

}

Measurement::Measurement(const Measurement &other)
    : id_(other.id_), data_(other.data_), recordedAt_(other.recordedAt_), sensor_(other.sensor_)
{

}

Measurement::Measurement(Measurement &&other)
    : id_(std::move(other.id_)), data_(std::move(other.data_)), recordedAt_(std::move(other.recordedAt_)),
    sensor_(std::move(other.sensor_))
{

}

void Measurement::setId(qint64 newId)
{
    id_ = newId;
}

void Measurement::setData(const QByteArray &newData)
{
    data_ = newData;
}

void Measurement::setRecordedAt(const QDateTime &newRecordedAt)
{
    recordedAt_ = newRecordedAt;
}

void Measurement::setSensorId(Sensor newSensor)
{
    sensor_ = newSensor;
}

qint64 Measurement::id() const
{
    return id_;
}

const QByteArray &Measurement::data() const
{
    return data_;
}

const QDateTime &Measurement::recordedAt() const
{
    return recordedAt_;
}

const Sensor &Measurement::sensor() const
{
    return sensor_;
}

QJsonObject Measurement::toJson() const
{
    QJsonObject json;
    json["id"] = id_;

    if (sensor_.type().name() == "temperature") {
        bool ok;
        double temperatureData = data_.toDouble(&ok);
        if (ok) {
            json["data"] = temperatureData;
        } else {
            json["data"] = QJsonValue::Null;
        }
    } else if (sensor_.type().name() == "power") {
        bool ok;
        double powerData = data_.toDouble(&ok);
        if (ok) {
            json["data"] = powerData;
        } else {
            json["data"] = QJsonValue::Null;
        }
    } else {
        json["data"] = QJsonValue::Null;
    }

    json["recorded_at"] = recordedAt_.toUTC().toMSecsSinceEpoch();
    return json;
}
