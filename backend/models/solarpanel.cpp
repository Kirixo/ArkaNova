#include "solarpanel.h"

SolarPanel::SolarPanel()
    : id_(-1), location_(QString()), user_(User()) {}

SolarPanel::SolarPanel(qint64 id, const QString& location, const User& user)
    : id_(id), location_(location), user_(user)
{}

SolarPanel::SolarPanel(const SolarPanel& other)
    : id_(other.id_), location_(other.location_), user_(other.user_)
{}

SolarPanel::SolarPanel(SolarPanel&& other) noexcept
    : id_(std::move(other.id_)), location_(std::move(other.location_)), user_(std::move(other.user_))
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

QJsonObject SolarPanel::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["location"] = location_;
    json["user_id"] = user_.id();
    return json;
}
