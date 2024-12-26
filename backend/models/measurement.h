#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QJsonObject>
#include "../utils/jsonable.h"
#include "../models/sensor.h"

class Measurement : public Jsonable {
public:
    Measurement();
    Measurement(quint64 id, QByteArray data, QDateTime recordedAt, Sensor sensor);
    Measurement(const Measurement& other);
    Measurement(Measurement&& other);
    Measurement &operator=(const Measurement &) = default;
    Measurement &operator=(Measurement &&) = default;


    void setId(qint64 newId);
    void setData(const QByteArray& newData);
    void setRecordedAt(const QDateTime& newRecordedAt);
    void setSensorId(Sensor newSensor);

    qint64 id() const;
    const QByteArray& data() const;
    const QDateTime& recordedAt() const;
    const Sensor& sensor() const;

    QJsonObject toJson() const override;

private:
    qint64 id_;
    QByteArray data_;
    QDateTime recordedAt_;
    Sensor sensor_;
};


#endif // MEASUREMENT_H
