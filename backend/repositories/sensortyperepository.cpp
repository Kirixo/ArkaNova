#include "sensortyperepository.h"
#include "../controllers/dbcontroller.h"
#include <qsqlerror.h>

std::optional<SensorType> SensorTypeRepository::fetchById(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, name
        FROM sensor_type
        WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        qint64 sensorTypeId = query.value("id").toLongLong();
        QString name = query.value("name").toString();

        return SensorType(sensorTypeId, name);
    }

    qDebug() << "Database error while fetching SensorType by ID:" << query.lastError().text();
    return std::nullopt;
}

