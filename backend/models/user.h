#ifndef USER_H
#define USER_H
#include "../utils/jsonable.h"

class User : Jsonable
{
public:
    User();
    User(quint64 id);
    User(quint64 id, const QString& email, const QString& password);
    User(const User& user);
    User(User&& user);
    User &operator=(const User &) = default;
    User &operator=(User &&) = default;

    void setId(qint64 newId);
    void setEmail(const QString &newEmail);
    void setPassword(const QString &newPassword);

    qint64 id() const;
    const QString& email() const;
    const QString& password () const;


private:
    qint64 id_ {-1};
    QString email_ {};
    QString password_ {};

    // Jsonable interface
public:
    QJsonObject toJson() const override;

};

#endif // USER_H
