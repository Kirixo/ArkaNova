#ifndef SOLARPANEL_H
#define SOLARPANEL_H

#include "../utils/jsonable.h" // Assuming this is your base class for JSON serialization
#include "../models/user.h"    // Assuming this is your User model
#include <QDateTime>           // For timestamp handling
#include <QJsonObject>         // For toJson method

class SolarPanel : public Jsonable // Ensure Jsonable is a public base if not already
{
public:
    SolarPanel();

    SolarPanel(qint64 id, const QString &location, const User &user, const QDateTime &createdAt, const QDateTime &updatedAt);
    SolarPanel(const SolarPanel &other);
    SolarPanel(SolarPanel &&other) noexcept;
    SolarPanel &operator=(const SolarPanel &) = default;
    SolarPanel &operator=(SolarPanel &&) = default;

    // Getters
    qint64 id() const;
    const QString& location() const;
    const User& user() const;
    const QDateTime& createdAt() const;
    const QDateTime& updatedAt() const;

    // Setters
    void setId(qint64 newId);
    void setLocation(const QString &newLocation);
    void setUser(const User &newUser);
    void setCreatedAt(const QDateTime &newCreatedAt);
    void setUpdatedAt(const QDateTime &newUpdatedAt);

private:
    qint64 id_;
    QString location_;
    User user_;
    QDateTime createdAt_;
    QDateTime updatedAt_;

public:
    // Jsonable interface
    QJsonObject toJson() const override;
};

#endif // SOLARPANEL_H
