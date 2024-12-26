#ifndef SENSORTYPE_H
#define SENSORTYPE_H
#include "../utils/jsonable.h"

class SensorType : Jsonable
{
public:
    SensorType();
    SensorType(qint64 id, const QString& name);
    SensorType(const SensorType& sensorType);
    SensorType(SensorType&& sensorType);
    SensorType &operator=(const SensorType &) = default;
    SensorType &operator=(SensorType &&) = default;

    qint64 id() const;
    void setId(qint64 newId);
    const QString& name() const;
    void setName(const QString &newName);

private:
    qint64 id_ {-1};
    QString name_ {};

    // Jsonable interface
public:
    QJsonObject toJson() const override;
};

#endif // SENSORTYPE_H
