#include "database.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

using namespace libface;

namespace kface
{

class DatabaseStatic
{
public:

    DatabaseStatic()
    {
    }

    QMutex  mutex;
    //QHash<QString, Database> hash;
};

class DatabasePriv : public QSharedData
{
public:

    DatabasePriv()
    {
        libface = 0;
    }

    LibFace *libface;
    QString  configPath;
};

Database::Database()
{
}

Database::Database(const Database& other)
{
}

Database::~Database()
{

}

};
