#ifndef BACKUPHANDLER_H
#define BACKUPHANDLER_H

#include <QHttpServerResponse>
#include <QHttpServerRequest>
#include <memory>

// Forward declare DBController
class DBController;

class BackupHandler
{
public:
    // Constructor now takes DBController to get connection parameters
    BackupHandler(std::shared_ptr<DBController> dbController);

    QHttpServerResponse exportDatabase(const QHttpServerRequest& request);
    QHttpServerResponse importDatabase(const QHttpServerRequest& request);

private:
    std::shared_ptr<DBController> dbController_;

    // Helper to get connection string or parameters from DBController
    // You might need to implement these in DBController or adjust how params are fetched
    QString getPgDumpConnectionString() const; // Example helper
    QString getPsqlConnectionString() const;   // Example helper
    QString getPassword() const; // To set PGPASSWORD
    QString getDbName() const;
    QString getUserName() const;
    QString getHostName() const;
    int getPort() const;
};

#endif // BACKUPHANDLER_H
