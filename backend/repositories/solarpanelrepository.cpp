#include "solarpanelrepository.h"
#include "../controllers/dbcontroller.h"
#include "userrepository.h"
#include <qsqlerror.h>

SolarPanelRepository::SolarPanelRepository() {}

std::optional<SolarPanel> SolarPanelRepository::fetchById(qint64 id)
{
    QSqlQuery query((DBController::getDatabase()));
    QString queryString = R"(
        SELECT id, location, user_id
        FROM solar_panel
        WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        UserRepository userRepository;
        qint64 SolarPanelId = query.value("id").toLongLong();
        QString location = query.value("location").toString();
        User user = userRepository.findUserById(query.value("user_id").toInt()).value();

        return SolarPanel(SolarPanelId, location, user);
    }

    qDebug() << "Database error while fetching SensorType by ID:" << query.lastError().text();
    return std::nullopt;
}


QList<SolarPanel> SolarPanelRepository::getPanelsByUser(qint64 userId, qint32 page, qint32 limit) {
    QList<SolarPanel> solarPanels;

    int offset = (page - 1) * limit;

    QSqlQuery query(DBController::getDatabase());
    query.prepare("SELECT id, location, user_id FROM solar_panel WHERE user_id = :user_id LIMIT :limit OFFSET :offset");
    query.bindValue(":user_id", userId);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (query.exec()) {
        UserRepository userRepository;

        while (query.next()) {
            auto user = userRepository.getUserById(userId);
            if (!user) {
                return {};
            }
            solarPanels.append(SolarPanel(query.value("id").toLongLong(),
                                          query.value("location").toString(),
                                          user.value()));
        }
    }
    return solarPanels;
}

bool SolarPanelRepository::deleteSolarPanel(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare("DELETE FROM solar_panel WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() > 0;
}

std::optional<SolarPanel> SolarPanelRepository::createSolarPanel(const SolarPanel& solarPanel) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare(R"(INSERT INTO solar_panel (location, user_id)
                    VALUES (:location, :user_id))");
    query.bindValue(":location", solarPanel.location());
    query.bindValue(":user_id", solarPanel.user().id());

    if (query.exec()) {
        qint64 newId = query.lastInsertId().toLongLong();

        return SolarPanel(newId, solarPanel.location(), solarPanel.user());
    }
    return std::nullopt;
}

bool SolarPanelRepository::updateSolarPanel(const SolarPanel& solarPanel) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare(R"(UPDATE solar_panel
                    SET location = :location,
                        user_id = :user_id,
                        updated_at = CURRENT_TIMESTAMP
                    WHERE id = :id)");
    query.bindValue(":location", solarPanel.location());
    query.bindValue(":user_id", solarPanel.user().id());
    query.bindValue(":id", solarPanel.id());
    return query.exec() && query.numRowsAffected() > 0;
}
