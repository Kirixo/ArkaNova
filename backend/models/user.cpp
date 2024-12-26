#include "user.h"

User::User()
    : id_(-1), email_(QString()), password_(QString())
{
}

User::User(quint64 id)
    : id_(id)
{

}

User::User(quint64 id, const QString &email, const QString &password)
    : id_(id), email_(email), password_(password)
{
}

User::User(const User &user)
    : id_(user.id_), email_(user.email_), password_(user.password_)
{

}

User::User(User &&user)
    : id_(std::move(user.id_)), email_(std::move(user.email_)), password_(std::move(user.password_))
{
}


qint64 User::id() const
{
    return id_;
}

void User::setId(qint64 newId)
{
    id_ = newId;
}

const QString& User::email() const
{
    return email_;
}

void User::setEmail(const QString &newEmail)
{
    email_ = newEmail;
}

const QString& User::password() const
{
    return password_;
}

void User::setPassword(const QString &newPassword)
{
    password_ = newPassword;
}

QJsonObject User::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["email"] = email_;
    return json;
}
