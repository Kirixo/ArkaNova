#include "userrepository.h"
#include "../controllers/dbcontroller.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <optional>

std::optional<User> UserRepository::getUserById(qint64 id) {
    QList<User> users;
    QString queryString = R"(
        SELECT id, email, password FROM "user"
        WHERE id = :id;
    )";
    QSqlQuery query(DBController::getDatabase());
    query.prepare(queryString);
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return User(query.value("id").toInt(),
                    query.value("email").toString(),
                    query.value("password").toString());
    } else {
        qDebug() << "Database error:" << query.lastError().text();
    }
    return std::nullopt;
}

bool UserRepository::updateUser(const User& user) {
    QString queryString = R"(UPDATE "user" SET )";
    QMap<QString, QVariant> fieldsToUpdate;

    if (!user.email().isEmpty()) {
        fieldsToUpdate[":email"] = user.email();
        queryString += "email = :email, ";
    }

    if (!user.password().isEmpty()) {
        fieldsToUpdate[":password"] = user.password();
        queryString += "password = :password, ";
    }

    queryString += "updated_at = NOW() WHERE id = :id";

    QSqlQuery query(DBController::getDatabase());
    query.prepare(queryString);
    query.bindValue(":id", user.id());

    for (auto it = fieldsToUpdate.begin(); it != fieldsToUpdate.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qWarning() << "Failed to update user:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}


bool UserRepository::createUser(const User& user) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        INSERT INTO "user" (email, password)
        VALUES (:email, :password)
    )";
    query.prepare(queryString);
    query.bindValue(":email", user.email());
    query.bindValue(":password", user.password()); // TODO: Ensure password is hashed before inserting
    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UserRepository::deleteUser(qint64 userId) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        DELETE FROM "user" WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", userId);
    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::optional<User> UserRepository::findUserByEmail(const QString& email) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user" WHERE email = :email
    )";
    query.prepare(queryString);
    query.bindValue(":email", email);
    if (query.exec() && query.next()) {
        return User(query.value("id").toLongLong(),
                    query.value("email").toString(),
                    query.value("password").toString());
    }
    qDebug() << "Database error:" << query.lastError().text();
    return std::nullopt;
}

std::optional<User> UserRepository::findUserById(qint64 id)
{
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user" WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return User(query.value("id").toLongLong(),
                    query.value("email").toString(),
                    query.value("password").toString());
    }
    qDebug() << "Database error:" << query.lastError().text();
    return std::nullopt;
}
