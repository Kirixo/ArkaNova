#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H
#include "../models/user.h"

class UserRepository
{
public:
    UserRepository() {};
    std::optional<User> getUserById(qint64 id);
    bool updateUser(const User& user);
    bool createUser(const User &user);
    bool deleteUser(qint64 userId);
    std::optional<User> findUserByEmail(const QString &email);
    std::optional<User> findUserById(qint64 id);
};

#endif // USERREPOSITORY_H
