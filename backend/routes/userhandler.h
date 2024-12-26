#ifndef USERHANDLER_H
#define USERHANDLER_H
#include <qhttpserverresponse.h>
#include "../repositories/userrepository.h"

class UserHandler
{
public:
    UserHandler();
    QHttpServerResponse getUser(const QHttpServerRequest& request);

    QHttpServerResponse updateUser(const QHttpServerRequest& request);
    QHttpServerResponse deleteUser(const QHttpServerRequest& request);
    QHttpServerResponse registerUser(const QHttpServerRequest& request);
    QHttpServerResponse loginUser(const QHttpServerRequest& request);
private:
    std::shared_ptr<UserRepository> userRepository_;
};

#endif // USERHANDLER_H
