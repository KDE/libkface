/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-04-15
 * Description : Abstract database backend
 *
 * Copyright (C) 2007-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DATABASECOREBACKEND_P_H
#define DATABASECOREBACKEND_P_H

// Qt includes

#include <QHash>
#include <QSqlDatabase>
#include <QThread>
#include <QWaitCondition>

// Local includes

#include "databaseparameters.h"

namespace KFaceIface
{

class DatabaseCoreBackendPrivate : public DatabaseErrorAnswer
{
public:

    DatabaseCoreBackendPrivate(DatabaseCoreBackend* const backend);
    virtual ~DatabaseCoreBackendPrivate() {}

    void init(const QString& connectionName, DatabaseLocking* const locking);

    QString connectionName(QThread* const thread);

    QSqlDatabase databaseForThread();
    QSqlError    databaseErrorForThread();
    void         setDatabaseErrorForThread(const QSqlError& lastError);

    void closeDatabaseForThread();
    bool open(QSqlDatabase& db);
    bool incrementTransactionCount();
    bool decrementTransactionCount();
    bool isInTransactionInOtherThread() const;

    bool isInMainThread() const;
    bool isInUIThread() const;

    bool reconnectOnError() const;
    bool isSQLiteLockError(const SqlQuery& query) const;
    bool isSQLiteLockTransactionError(const QSqlError& lastError) const;
    bool checkRetrySQLiteLockError(int retries);
    bool isConnectionError(const SqlQuery& query) const;
    bool needToConsultUserForError(const SqlQuery& query) const;
    bool needToHandleWithErrorHandler(const SqlQuery& query) const;
    void debugOutputFailedQuery(const QSqlQuery& query) const;
    void debugOutputFailedTransaction(const QSqlError& error) const;

    bool checkOperationStatus();
    bool handleWithErrorHandler(const SqlQuery* const query);
    void setQueryOperationFlag(DatabaseCoreBackend::QueryOperationStatus status);
    void queryOperationWakeAll(DatabaseCoreBackend::QueryOperationStatus status);

    // called by DatabaseErrorHandler, implementing DatabaseErrorAnswer
    virtual void connectionErrorContinueQueries();
    virtual void connectionErrorAbortQueries();

    virtual void transactionFinished();

public:

    // this is always accessed in mutex context, no need for QThreadStorage
    QHash<QThread*, QSqlDatabase>             threadDatabases;
    // this is not only db.isValid(), but also "parameters changed, need to reopen"
    QHash<QThread*, int>                      databasesValid;
    // for recursive transactions
    QHash<QThread*, int>                      transactionCount;

    QHash<QThread*, QSqlError>                databaseErrors;

    bool                                      isInTransaction;

    QString                                   backendName;

    DatabaseParameters                        parameters;

    DatabaseCoreBackend::Status               status;

    DatabaseLocking*                          lock;

    DatabaseCoreBackend::QueryOperationStatus operationStatus;

    QMutex                                    errorLockMutex;
    QWaitCondition                            errorLockCondVar;
    DatabaseCoreBackend::QueryOperationStatus errorLockOperationStatus;

    QMutex                                    busyWaitMutex;
    QWaitCondition                            busyWaitCondVar;

    DatabaseErrorHandler*                     errorHandler;

public :

    class AbstractUnlocker
    {
    public:

        AbstractUnlocker(DatabaseCoreBackendPrivate* const d);
        ~AbstractUnlocker();

        void finishAcquire();

    protected:

        int                               count;
        DatabaseCoreBackendPrivate* const d;
    };

    friend class AbstractUnlocker;

    // ------------------------------------------------------------------

    class AbstractWaitingUnlocker : public AbstractUnlocker
    {
    public:

        AbstractWaitingUnlocker(DatabaseCoreBackendPrivate* const d, QMutex* const mutex, QWaitCondition* const condVar);
        ~AbstractWaitingUnlocker();

        bool wait(unsigned long time = ULONG_MAX);

    protected:

        QMutex*         const mutex;
        QWaitCondition* const condVar;
    };

    // ------------------------------------------------------------------

    class ErrorLocker : public AbstractWaitingUnlocker
    {
    public:

        ErrorLocker(DatabaseCoreBackendPrivate* const d);
        void wait();
    };

    // ------------------------------------------------------------------

    class BusyWaiter : public AbstractWaitingUnlocker
    {
    public:

        BusyWaiter(DatabaseCoreBackendPrivate* const d);
    };

public :

    DatabaseCoreBackend* const q;
};

} // namespace KFaceIface

#endif // DATABASECOREBACKEND_P_H
