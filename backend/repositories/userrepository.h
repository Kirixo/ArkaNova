#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../models/user.h"
#include <QList>      // Required for QList
#include <optional>   // Required for std::optional

class UserRepository
{
public:
    UserRepository() {};
    std::optional<User> getUserById(qint64 id);
    bool updateUser(const User& user);
    std::optional<User> createUser(const User &user); // Changed to return optional<User> for consistency
    bool deleteUser(qint64 userId);
    std::optional<User> findUserByEmail(const QString &email);
    std::optional<User> findUserById(qint64 id);

    // New methods for listing users with pagination
    QList<User> getUsers(int page, int limit);
    int getTotalUserCount();
    bool verifyPassword(const QString& email, const QString& password); // Added declaration
};

#endif // USERREPOSITORY_H
