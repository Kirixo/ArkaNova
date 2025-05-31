#ifndef USERHANDLER_H
#define USERHANDLER_H

#include <QHttpServerResponse> // Correct include
#include <QHttpServerRequest>  // Required for request parameter
#include "../repositories/userrepository.h" // Ensure path is correct
#include <memory> // Required for std::shared_ptr

class UserHandler
{
public:
    UserHandler();
    QHttpServerResponse getUser(const QHttpServerRequest& request);
    QHttpServerResponse updateUser(const QHttpServerRequest& request);
    QHttpServerResponse deleteUser(const QHttpServerRequest& request);
    QHttpServerResponse registerUser(const QHttpServerRequest& request);
    QHttpServerResponse loginUser(const QHttpServerRequest& request);

    // New method for listing users
    QHttpServerResponse getUserList(const QHttpServerRequest& request);

private:
    std::shared_ptr<UserRepository> userRepository_;
};

#endif // USERHANDLER_H
