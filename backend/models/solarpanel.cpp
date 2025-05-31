#include "solarpanel.h"
#include <utility> // For std::move

SolarPanel::SolarPanel()
    : id_(-1), location_(QString()), user_(User()), createdAt_(QDateTime()), updatedAt_(QDateTime()) {}

SolarPanel::SolarPanel(qint64 id, const QString& location, const User& user, const QDateTime& createdAt, const QDateTime& updatedAt)
    : id_(id), location_(location), user_(user), createdAt_(createdAt), updatedAt_(updatedAt)
{}

SolarPanel::SolarPanel(const SolarPanel& other)
    : id_(other.id_),
    location_(other.location_),
    user_(other.user_),
    createdAt_(other.createdAt_),
    updatedAt_(other.updatedAt_)
{}

SolarPanel::SolarPanel(SolarPanel&& other) noexcept
    : id_(other.id_), // For POD types like qint64, std::move is like a copy
    location_(std::move(other.location_)),
    user_(std::move(other.user_)),
    createdAt_(std::move(other.createdAt_)),
    updatedAt_(std::move(other.updatedAt_))
{}

qint64 SolarPanel::id() const
{
    return id_;
}

void SolarPanel::setId(qint64 newId)
{
    id_ = newId;
}

const QString& SolarPanel::location() const
{
    return location_;
}

void SolarPanel::setLocation(const QString &newLocation)
{
    location_ = newLocation;
}

const User& SolarPanel::user() const
{
    return user_;
}

void SolarPanel::setUser(const User &newUser)
{
    user_ = newUser;
}

const QDateTime& SolarPanel::createdAt() const
{
    return createdAt_;
}

void SolarPanel::setCreatedAt(const QDateTime &newCreatedAt)
{
    createdAt_ = newCreatedAt;
}

const QDateTime& SolarPanel::updatedAt() const
{
    return updatedAt_;
}

void SolarPanel::setUpdatedAt(const QDateTime &newUpdatedAt)
{
    updatedAt_ = newUpdatedAt;
}

QJsonObject SolarPanel::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["location"] = location_;
    json["user_id"] = user_.id(); // Assuming user_.id() correctly returns the user's ID
    json["created_at"] = createdAt_.toSecsSinceEpoch();
    json["updated_at"] = updatedAt_.toSecsSinceEpoch(); // QDateTime().toSecsSinceEpoch() returns 0 if invalid
    return json;
}
