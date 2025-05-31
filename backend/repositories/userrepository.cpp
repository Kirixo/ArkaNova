#include "userrepository.h"
#include "../controllers/dbcontroller.h" // Ensure this path is correct
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <optional>
// #include <QCryptographicHash> // Commented out as per request

std::optional<User> UserRepository::getUserById(qint64 id) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user"
        WHERE id = :id;
    )";
    query.prepare(queryString);
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return User(query.value("id").toLongLong(),
                    query.value("email").toString(),
                    query.value("password").toString()); // Use 'password' column
    } else {
        if(query.lastError().isValid())
            qDebug() << "Database error (getUserById):" << query.lastError().text();
    }
    return std::nullopt;
}

bool UserRepository::updateUser(const User& user) {
    QSqlQuery query(DBController::getDatabase());
    // Note: This update logic assumes you might want to update email and/or password.
    // If password is provided in the User object, it will be updated.
    // If only email is being updated, the password in the User object should be the existing one or empty.
    QString queryString = R"(UPDATE "user" SET email = :email)";
    bool passwordProvided = !user.password().isEmpty();

    if (passwordProvided) {
        queryString += ", password = :password"; // Update 'password' column
    }
    queryString += R"(, updated_at = NOW() WHERE id = :id)";

    query.prepare(queryString);
    query.bindValue(":email", user.email());
    if (passwordProvided) {
        query.bindValue(":password", user.password());
    }
    query.bindValue(":id", user.id());

    if (!query.exec()) {
        qWarning() << "Failed to update user:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

std::optional<User> UserRepository::createUser(const User& user) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        INSERT INTO "user" (email, password)
        VALUES (:email, :password) RETURNING id;
    )"; // Use 'password' column
    query.prepare(queryString);

    // --- Hashing Logic Commented Out ---
    // QByteArray passwordData = user.password().toUtf8();
    // QByteArray salt = "someRandomSalt"; // Salt should be unique per user and stored
    // QByteArray saltedPassword = passwordData + salt;
    // QString hashedPassword = QString(QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256).toHex());
    // query.bindValue(":password", hashedPassword); // Store the hashed password
    // --- End of Hashing Logic ---

    // Store plain password as per current setup
    query.bindValue(":email", user.email());
    query.bindValue(":password", user.password()); // Store plain password

    if (query.exec()) {
        if (query.next()) {
            qint64 newId = query.value(0).toLongLong();
            // Return user with new ID and the plain password provided
            return User(newId, user.email(), user.password());
        }
    } else {
        qDebug() << "Database error (createUser):" << query.lastError().text();
    }
    return std::nullopt;
}

bool UserRepository::deleteUser(qint64 userId) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        DELETE FROM "user" WHERE id = :id
    )";
    query.prepare(queryString);
    query.bindValue(":id", userId);
    if (!query.exec()) {
        qDebug() << "Database error (deleteUser):" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

std::optional<User> UserRepository::findUserByEmail(const QString& email) {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user" WHERE email = :email
    )"; // Use 'password' column
    query.prepare(queryString);
    query.bindValue(":email", email);
    if (query.exec() && query.next()) {
        return User(query.value("id").toLongLong(),
                    query.value("email").toString(),
                    query.value("password").toString()); // Use 'password' column
    }
    if(query.lastError().isValid())
        qDebug() << "Database error (findUserByEmail):" << query.lastError().text();
    return std::nullopt;
}

std::optional<User> UserRepository::findUserById(qint64 id)
{
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user" WHERE id = :id
    )"; // Use 'password' column
    query.prepare(queryString);
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return User(query.value("id").toLongLong(),
                    query.value("email").toString(),
                    query.value("password").toString()); // Use 'password' column
    }
    if(query.lastError().isValid())
        qDebug() << "Database error (findUserById):" << query.lastError().text();
    return std::nullopt;
}

QList<User> UserRepository::getUsers(int page, int limit) {
    QList<User> users;
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(
        SELECT id, email, password FROM "user"
        ORDER BY id ASC
        LIMIT :limit OFFSET :offset;
    )"; // Use 'password' column
    query.prepare(queryString);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", (page - 1) * limit);

    if (query.exec()) {
        while (query.next()) {
            users.append(User(query.value("id").toLongLong(),
                              query.value("email").toString(),
                              query.value("password").toString())); // Use 'password' column
        }
    } else {
        qDebug() << "Database error (getUsers):" << query.lastError().text();
    }
    return users;
}

int UserRepository::getTotalUserCount() {
    QSqlQuery query(DBController::getDatabase());
    QString queryString = R"(SELECT COUNT(*) FROM "user";)";
    query.prepare(queryString);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qDebug() << "Database error (getTotalUserCount):" << query.lastError().text();
    }
    return 0;
}

bool UserRepository::verifyPassword(const QString& email, const QString& plainPassword) {
    auto userOptional = findUserByEmail(email);
    if (userOptional) {
        QString storedPassword = userOptional->password(); // This is the stored plain password

        // --- Hashing Logic Commented Out ---
        // QByteArray passwordData = plainPassword.toUtf8();
        // QByteArray salt = "someRandomSalt"; // Salt should be retrieved alongside the hash for the user
        // QByteArray saltedPassword = passwordData + salt;
        // QString hashedAttempt = QString(QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256).toHex());
        // return storedHash == hashedAttempt; // Compare hashes
        // --- End of Hashing Logic ---

        // Direct comparison for plain text passwords
        // WARNING: This is insecure. Passwords should be hashed.
        return storedPassword == plainPassword;
    }
    return false;
}
