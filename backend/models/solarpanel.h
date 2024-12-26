#ifndef SOLARPANEL_H
#define SOLARPANEL_H
#include "../utils/jsonable.h"
#include "../models/user.h"

class SolarPanel : Jsonable
{
public:
    SolarPanel();

    SolarPanel(qint64 id, const QString &location, const User &user);
    SolarPanel(const SolarPanel &other);
    SolarPanel(SolarPanel &&other) noexcept;
    SolarPanel &operator=(const SolarPanel &) = default;
    SolarPanel &operator=(SolarPanel &&) = default;

    qint64 id() const;
    void setId(qint64 newId);
    const QString& location() const;
    void setLocation(const QString &newLocation);
    const User& user() const;
    void setUser(const User &newUser);

private:
    qint64 id_;
    QString location_;
    User user_;

    // Jsonable interface
public:
    QJsonObject toJson() const override;


};

#endif // SOLARPANEL_H
