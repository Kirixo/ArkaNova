#include "backuphandler.h"
#include "../utils/responsefactory.h"
#include "../controllers/dbcontroller.h" // For DB connection parameters

#include <QProcess>
#include <QTemporaryFile>
#include <QJsonDocument> // For error responses
#include <QJsonObject>   // For error responses
#include <QDir>          // For temporary file path
#include <QFile>         // For reading temp file for logging
#include <QTextStream>   // For reading temp file for logging
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QDebug>

BackupHandler::BackupHandler(std::shared_ptr<DBController> dbController)
    : dbController_(dbController)
{
    if (!dbController_) {
        qCritical() << "BackupHandler initialized with null DBController!";
    }
}

// Example helper implementations (these should ideally pull from DBController's actual config)
QString BackupHandler::getDbName() const {
    return DBController::getDatabase().databaseName();
}
QString BackupHandler::getUserName() const {
    return DBController::getDatabase().userName();
}
QString BackupHandler::getPassword() const {
    return DBController::getDatabase().password();
}
QString BackupHandler::getHostName() const {
    return DBController::getDatabase().hostName().isEmpty() ? "localhost" : DBController::getDatabase().hostName();
}
int BackupHandler::getPort() const {
    return DBController::getDatabase().port() == -1 ? 5432 : DBController::getDatabase().port() ;
}


QHttpServerResponse BackupHandler::exportDatabase(const QHttpServerRequest& request) {
    (void)request;

    if (!dbController_) {
        return ResponseFactory::createErrorResponse("Database controller not available.", QHttpServerResponse::StatusCode::InternalServerError);
    }

    QProcess pgDumpProcess;
    QStringList arguments;

    // Simplified pg_dump arguments, removing options that might not be supported by older versions
    arguments << "--dbname=" + getDbName();
    arguments << "--host=" + getHostName();
    arguments << "--port=" + QString::number(getPort());
    arguments << "--username=" + getUserName();
    arguments << "--format=plain"; // Plain SQL script is generally safest
    arguments << "--clean";        // Include DROP statements
    arguments << "--if-exists";    // Add IF EXISTS to DROP statements
    // Removed: --no-runtime-configuration, --no-unlogged-table-data, --disable-dollar-quoting, --quote-all-identifiers
    // as --no-runtime-configuration was reported as illegal.

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PGPASSWORD", getPassword());
    pgDumpProcess.setProcessEnvironment(env);

    qDebug() << "Starting pg_dump with arguments:" << arguments.join(" ");
    pgDumpProcess.start("pg_dump", arguments);

    if (!pgDumpProcess.waitForStarted()) {
        qCritical() << "Failed to start pg_dump:" << pgDumpProcess.errorString();
        return ResponseFactory::createErrorResponse("Failed to start database dump process.",
                                                    QHttpServerResponse::StatusCode::InternalServerError);
    }

    QByteArray dumpData;
    QByteArray errorData;

    if (pgDumpProcess.waitForFinished(-1)) { // Wait indefinitely
        dumpData = pgDumpProcess.readAllStandardOutput();
        errorData = pgDumpProcess.readAllStandardError();

        if (pgDumpProcess.exitStatus() == QProcess::NormalExit && pgDumpProcess.exitCode() == 0) {
            if (!errorData.isEmpty() && !errorData.contains("discarding old contents of")) { // pg_dump can have verbose stderr
                qWarning() << "pg_dump stderr (export):" << QString::fromUtf8(errorData);
            }

            QString dumpString = QString::fromUtf8(dumpData);
            qDebug().noquote() << "Dump content (first 1000 chars) BEFORE regex processing:\n" << dumpString.left(1000);

            // Regex to find and remove "SET transaction_timeout = ...;"
            // Making it robust for whitespace and ensuring it's anchored to line start/end for SET commands
            QRegularExpression problematicSetCommand(R"(^\s*SET\s+transaction_timeout\s*=\s*[^;]+;\s*$\n?)",
                                                     QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);

            int initialLength = dumpString.length();
            dumpString.remove(problematicSetCommand); // remove() modifies in place
            int finalLength = dumpString.length();

            if (initialLength != finalLength) {
                qInfo() << "Regex attempted to remove 'SET transaction_timeout'. Original length:" << initialLength << "New length:" << finalLength;
            } else {
                qInfo() << "No 'SET transaction_timeout' found by regex for removal, or regex did not change string length.";
            }

            qDebug().noquote() << "Dump content (first 1000 chars) AFTER regex processing:\n" << dumpString.left(1000);

            if (dumpString.contains("SET transaction_timeout", Qt::CaseInsensitive)) {
                qWarning().noquote() << "CRITICAL: 'SET transaction_timeout' STILL PRESENT in dumpString after regex removal attempt! Dump (first 1500 chars):\n" << dumpString.left(1500);
            } else {
                qInfo() << "'SET transaction_timeout' no longer found in dumpString after regex processing.";
            }

            dumpData = dumpString.toUtf8();


            qInfo() << "Database export successful. Final dump size:" << dumpData.size();
            QHttpServerResponse response(dumpData, QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Content-Type", "application/sql");
            response.setHeader("Content-Disposition", "attachment; filename=\"database_backup.sql\"");
            ResponseFactory::addCorsHeaders(response);
            return response;
        } else {
            qCritical() << "pg_dump process failed. Exit code:" << pgDumpProcess.exitCode()
            << "Exit status:" << pgDumpProcess.exitStatus()
            << "Error:" << pgDumpProcess.errorString()
            << "stderr:" << QString::fromUtf8(errorData)
            << "stdout (first 500B):" << QString::fromUtf8(dumpData.left(500));
            return ResponseFactory::createErrorResponse("Database dump process failed: " + QString::fromUtf8(errorData),
                                                        QHttpServerResponse::StatusCode::InternalServerError);
        }
    } else {
        qCritical() << "pg_dump process timed out or crashed:" << pgDumpProcess.errorString();
        errorData = pgDumpProcess.readAllStandardError();
        return ResponseFactory::createErrorResponse("Database dump process timed out or crashed: " + QString::fromUtf8(errorData),
                                                    QHttpServerResponse::StatusCode::InternalServerError);
    }
}

QHttpServerResponse BackupHandler::importDatabase(const QHttpServerRequest& request) {
    if (!dbController_) {
        return ResponseFactory::createErrorResponse("Database controller not available.", QHttpServerResponse::StatusCode::InternalServerError);
    }

    QByteArray backupFileContent = request.body();
    if (backupFileContent.isEmpty()) {
        return ResponseFactory::createErrorResponse("No backup file content received.",
                                                    QHttpServerResponse::StatusCode::BadRequest);
    }

    QTemporaryFile tempFileObj(QDir::tempPath() + "/uploaded_backup_XXXXXX.sql");
    tempFileObj.setAutoRemove(true);

    if (!tempFileObj.open()) {
        qCritical() << "Failed to create temporary file for import:" << tempFileObj.errorString();
        return ResponseFactory::createErrorResponse("Failed to create temporary file for import.",
                                                    QHttpServerResponse::StatusCode::InternalServerError);
    }
    qint64 bytesWritten = tempFileObj.write(backupFileContent);
    QString tempFilePath = tempFileObj.fileName();

    if (bytesWritten != backupFileContent.size()) {
        qCritical() << "Failed to write all data to temporary file" << tempFilePath << ". Wrote" << bytesWritten << "expected" << backupFileContent.size();
        tempFileObj.close();
        return ResponseFactory::createErrorResponse("Failed to write backup data to temporary file.", QHttpServerResponse::StatusCode::InternalServerError);
    }
    tempFileObj.close();

    qInfo() << "Backup data written to temporary file:" << tempFilePath;
    qInfo() << "Verifying first few lines of temporary file content for psql import:";
    QFile checkFile(tempFilePath);
    if (checkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&checkFile);
        for(int i = 0; i < 20 && !in.atEnd(); ++i) { // Log first 20 lines
            qDebug().noquote() << "TEMP_FILE_LINE " << i+1 << ":" << in.readLine();
        }
        checkFile.close();
    } else {
        qWarning() << "Could not open temporary file for verification:" << tempFilePath;
    }


    QProcess psqlProcess;
    QStringList arguments;

    arguments << "--dbname=" + getDbName();
    arguments << "--host=" + getHostName();
    arguments << "--port=" + QString::number(getPort());
    arguments << "--username=" + getUserName();
    arguments << "--file=" + tempFilePath;
    arguments << "--single-transaction"; // Execute as a single transaction
    arguments << "-v";                   // Set variable
    arguments << "ON_ERROR_STOP=1";      // Abort script on error

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PGPASSWORD", getPassword());
    psqlProcess.setProcessEnvironment(env);

    qDebug() << "Starting psql with arguments:" << arguments.join(" ");
    psqlProcess.start("psql", arguments);

    if (!psqlProcess.waitForStarted()) {
        qCritical() << "Failed to start psql:" << psqlProcess.errorString();
        return ResponseFactory::createErrorResponse("Failed to start database import process.",
                                                    QHttpServerResponse::StatusCode::InternalServerError);
    }

    QByteArray errorData;
    QByteArray outputData;

    if (psqlProcess.waitForFinished(-1)) { // Wait indefinitely
        outputData = psqlProcess.readAllStandardOutput();
        errorData = psqlProcess.readAllStandardError();

        if (!outputData.isEmpty()){
            qDebug().noquote() << "psql stdout (import):\n" << QString::fromUtf8(outputData);
        }

        if (psqlProcess.exitStatus() == QProcess::NormalExit && psqlProcess.exitCode() == 0) {
            qInfo() << "Database import successful.";
            if (!errorData.isEmpty()){
                qWarning().noquote() << "psql stderr (import success):\n" << QString::fromUtf8(errorData);
            }
            return ResponseFactory::createResponse("Database imported successfully.", QHttpServerResponse::StatusCode::Ok);
        } else {
            qCritical() << "psql process failed. Exit code:" << psqlProcess.exitCode()
            << "Exit status:" << psqlProcess.exitStatus()
            << "Error:" << psqlProcess.errorString();
            qCritical().noquote() << "psql stderr (import failure):\n" << QString::fromUtf8(errorData);
            return ResponseFactory::createErrorResponse("Database import process failed: " + QString::fromUtf8(errorData),
                                                        QHttpServerResponse::StatusCode::InternalServerError);
        }
    } else {
        qCritical() << "psql process timed out or crashed:" << psqlProcess.errorString();
        errorData = psqlProcess.readAllStandardError(); // Try to get any error output
        qCritical().noquote() << "psql stderr (timeout/crash):\n" << QString::fromUtf8(errorData);
        return ResponseFactory::createErrorResponse("Database import process timed out or crashed: " + QString::fromUtf8(errorData),
                                                    QHttpServerResponse::StatusCode::InternalServerError);
    }
}
