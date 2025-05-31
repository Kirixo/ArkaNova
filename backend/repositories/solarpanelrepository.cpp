#include "solarpanelrepository.h"
#include "../controllers/dbcontroller.h" // Ensure this path is correct
#include "userrepository.h"             // Ensure this path is correct
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

SolarPanelRepository::SolarPanelRepository() {}

std::optional<SolarPanel> SolarPanelRepository::fetchById(qint64 id)
{
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, location, user_id, created_at, updated_at
        FROM solar_panel
        WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        UserRepository userRepository;
        qint64 solarPanelId = query.value("id").toLongLong();
        QString location = query.value("location").toString();

        auto optionalUser = userRepository.findUserById(query.value("user_id").toInt());
        if (!optionalUser) { // Use .has_value() or implicit bool conversion
            qDebug() << "User not found for user_id:" << query.value("user_id").toInt() << "for SolarPanel ID:" << id;
            return std::nullopt;
        }
        User user = optionalUser.value();

        QDateTime createdAt = query.value("created_at").toDateTime();
        QDateTime updatedAt = query.value("updated_at").toDateTime();

        return SolarPanel(solarPanelId, location, user, createdAt, updatedAt);
    }

    qDebug() << "Database error while fetching SolarPanel by ID (" << id << "):" << query.lastError().text();
    return std::nullopt;
}

QList<SolarPanel> SolarPanelRepository::getPanelsByUser(qint64 userId, qint32 page, qint32 limit) {
    QList<SolarPanel> solarPanels;
    int offset = (page - 1) * limit;

    QSqlQuery query(DBController::getDatabase());
    query.prepare("SELECT id, location, user_id, created_at, updated_at FROM solar_panel WHERE user_id = :user_id LIMIT :limit OFFSET :offset");
    query.bindValue(":user_id", userId);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    UserRepository userRepository; // Instantiate once
    auto optionalMasterUser = userRepository.getUserById(userId);

    if (!optionalMasterUser) {
        qDebug() << "Master user with ID:" << userId << " not found. Cannot fetch panels.";
        return {}; // Return empty list if the primary user for whom panels are being fetched doesn't exist
    }
    User masterUser = optionalMasterUser.value();

    if (query.exec()) {
        while (query.next()) {
            QDateTime createdAt = query.value("created_at").toDateTime();
            QDateTime updatedAt = query.value("updated_at").toDateTime();

            solarPanels.append(SolarPanel(query.value("id").toLongLong(),
                                          query.value("location").toString(),
                                          masterUser, // Use the pre-fetched masterUser object
                                          createdAt,
                                          updatedAt));
        }
    } else {
        qDebug() << "Database error while fetching SolarPanels for user ID (" << userId << "):" << query.lastError().text();
    }
    return solarPanels;
}

bool SolarPanelRepository::deleteSolarPanel(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    query.prepare("DELETE FROM solar_panel WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Database error while deleting SolarPanel by ID (" << id << "):" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

std::optional<SolarPanel> SolarPanelRepository::createSolarPanel(const SolarPanel& solarPanel) {
    QSqlQuery query(DBController::getDatabase());
    // Database triggers (trg_solar_panel_insert and set_timestamps) will handle created_at and updated_at
    query.prepare(R"(INSERT INTO solar_panel (location, user_id)
                    VALUES (:location, :user_id))");
    query.bindValue(":location", solarPanel.location());
    query.bindValue(":user_id", solarPanel.user().id());

    if (query.exec()) {
        QVariant lastIdVariant = query.lastInsertId();
        if (lastIdVariant.isValid() && lastIdVariant.toLongLong() > 0) {
            qint64 newId = lastIdVariant.toLongLong();
            // Fetch the newly created panel to get all fields, including DB-generated timestamps
            return fetchById(newId);
        } else {
            qDebug() << "Failed to retrieve lastInsertId or invalid ID after creating SolarPanel. DB Error (if any):" << query.lastError().text() << "LastInsertID Variant:" << lastIdVariant;
        }
    }
    qDebug() << "Database error while creating SolarPanel:" << query.lastError().text();
    return std::nullopt;
}

bool SolarPanelRepository::updateSolarPanel(const SolarPanel& solarPanel) {
    QSqlQuery query(DBController::getDatabase());
    // The trigger trg_solar_panel_update and its procedure set_timestamps() will handle updated_at.
    // Explicitly setting it in the query is also fine and common (as in your original code).
    query.prepare(R"(UPDATE solar_panel
                    SET location = :location,
                        user_id = :user_id,
                        updated_at = CURRENT_TIMESTAMP
                    WHERE id = :id)");
    query.bindValue(":location", solarPanel.location());
    query.bindValue(":user_id", solarPanel.user().id());
    query.bindValue(":id", solarPanel.id());

    if(!query.exec()){
        qDebug() << "Database error while updating SolarPanel by ID (" << solarPanel.id() << "):" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
