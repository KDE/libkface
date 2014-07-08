/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-06-16
 * @brief Wrapper class for face recognition database
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// OpenCV includes need to show up before Qt includes
#include "recognition-opencv-lbph/opencvlbphfacerecognizer.h"
#include "preprocessing-tantriggs/tantriggspreprocessor.h"

#include "recognitiondatabase.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>
#include <QUuid>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

// Local includes

#include "databaseaccess.h"
#include "databaseoperationgroup.h"
#include "databaseparameters.h"
#include "dataproviders.h"
#include "funnelreal.h"
#include "trainingdb.h"
#include "version.h"

namespace KFaceIface
{

/**
 * The RecognitionDatabaseStaticPriv holds a hash to all exising RecognitionDatabase data,
 * mutex protected.
 * When creating a RecognitionDatabase, either a new Private is created,
 * or an existing one is used.
 * When the last RecognitionDatabase referencing a Private is destroyed,
 * the Private is destroyed as well, removing itself from the static hash.
 */
class RecognitionDatabaseStaticPriv
{
public:

    RecognitionDatabaseStaticPriv()
        : mutex(QMutex::Recursive)
    {
        // Note: same line in Database. Keep in sync.
        defaultPath = KStandardDirs::locateLocal("data", "libkface/database/", true);
    }

    QExplicitlySharedDataPointer<RecognitionDatabase::Private> database(const QString& key);
    void removeDatabase(const QString& key);

public:

    QString                                               defaultPath;
    QMutex                                                mutex;

    // Important: Do not hold an QExplicitlySharedDataPointer here, or the objects will never be freed!
    typedef QHash<QString, RecognitionDatabase::Private*> DatabaseHash;
    DatabaseHash                                          databases;
};

K_GLOBAL_STATIC(RecognitionDatabaseStaticPriv, static_d)

// -------------------------------------------------------------------------------------------------

class RecognitionDatabase::Private : public QSharedData
{
public:

    bool                 dbAvailable;

    const QString        configPath;
    QMutex               mutex;
    DatabaseAccessData*  db;

    QVariantMap          parameters;
    QHash<int, Identity> identityCache;

public:

    ~Private();

    template <class T>
    T* getObjectOrCreate(T* &ptr) const
    {
        if (!ptr)
        {
            ptr = new T(db);
        }

        return ptr;
    }

    // Change these three lines to change CurrentRecognizer
    typedef OpenCVLBPHFaceRecognizer CurrentRecognizer;
    CurrentRecognizer* recognizer()             { return getObjectOrCreate(opencvlbph); }
    CurrentRecognizer* recognizerConst()  const { return opencvlbph;                    }

    OpenCVLBPHFaceRecognizer* lbph()            { return getObjectOrCreate(opencvlbph); }
    OpenCVLBPHFaceRecognizer* lbphConst() const { return opencvlbph;                    }

    typedef FunnelReal CurrentAligner;
    CurrentAligner*    aligner();
    CurrentAligner*    alignerConst()     const { return funnel;                        }

    void applyParameters();

public:

    void train(OpenCVLBPHFaceRecognizer* const r, const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data, const QString& trainingContext);
    void clear(OpenCVLBPHFaceRecognizer* const, const QList<int>& idsToClear, const QString& trainingContext);

    cv::Mat preprocessingChain(const QImage& image);

public:

    bool identityContains(const Identity& identity, const QString& attribute, const QString& value) const;
    Identity findByAttribute(const QString& attribute, const QString& value) const;
    Identity findByAttributes(const QString& attribute, const QMap<QString, QString>& valueMap) const;

private:

    friend class RecognitionDatabaseStaticPriv;
    // Protected creation by StaticPriv only
    Private(const QString& configPath);

private:

    OpenCVLBPHFaceRecognizer* opencvlbph;
    FunnelReal*               funnel;
};

// -------------------------------------------------------------------------------------------------

QExplicitlySharedDataPointer<RecognitionDatabase::Private> RecognitionDatabaseStaticPriv::database(const QString& path)
{
    QMutexLocker lock(&mutex);
    QString configPath        = path.isNull() ? defaultPath : path;
    DatabaseHash::iterator it = databases.find(configPath);

    if (it != databases.end())
    {
        /* There is a race condition: The last Priv is dereferenced, the destructor called.
         * Now database() has been called and locks the mutex after this dereferencing, but before removeDatabase is executed.
         * So we only can use this cached data if its reference count is non-zero.
         * Atomically to testing, we increase the reference count to reserve it for our usage.
         */

        if (it.value()->ref.fetchAndAddOrdered(1) != 0)
        {
            QExplicitlySharedDataPointer<RecognitionDatabase::Private> p(it.value());
            it.value()->ref.deref(); // We incremented above
            return p;
        }

        /* if the original value is 0, it is currently being deleted, but it must be
         * safe to access it, because the destructor has not yet completed - otherwise it'd not be in the hash.
         */
    }

    RecognitionDatabase::Private* const d = new RecognitionDatabase::Private(configPath);
    databases[configPath]                 = d;

    return QExplicitlySharedDataPointer<RecognitionDatabase::Private>(d);
}

void RecognitionDatabaseStaticPriv::removeDatabase(const QString& key)
{
    QMutexLocker lock(&mutex);
    databases.remove(key);
}

// ----------------------------------------------------------------------------------------------

RecognitionDatabase::Private::Private(const QString& configPath)
    : configPath(configPath),
      mutex(QMutex::Recursive),
      db(DatabaseAccess::create()),
      opencvlbph(0),
      funnel(0)
{
    DatabaseParameters params = DatabaseParameters::parametersForSQLite(configPath + "/" + "recognition.db");
    DatabaseAccess::setParameters(db, params);
    dbAvailable               = DatabaseAccess::checkReadyForUse(db);

    if (dbAvailable)
    {
        foreach (const Identity& identity, DatabaseAccess(db).db()->identities())
        {
            identityCache[identity.id] = identity;
        }
    }
}

RecognitionDatabase::Private::~Private()
{
    delete opencvlbph;
    delete funnel;

    static_d->removeDatabase(configPath);
    DatabaseAccess::destroy(db);
}

RecognitionDatabase::Private::CurrentAligner* RecognitionDatabase::Private::aligner()
{
    if (!funnel)
    {
        funnel = new FunnelReal;
    }

    return funnel;
}

// other RecognitionDatabase::Private methods are to be found below, in the relevant context of the main class

// -------------------------------------------------------------------------------------------------

RecognitionDatabase::RecognitionDatabase()
{
}

RecognitionDatabase::RecognitionDatabase(QExplicitlySharedDataPointer<Private> d)
    : d(d)
{
}

RecognitionDatabase::RecognitionDatabase(const RecognitionDatabase& other)
{
    d = other.d;
}

RecognitionDatabase& RecognitionDatabase::operator=(const KFaceIface::RecognitionDatabase& other)
{
    d = other.d;
    return *this;
}

RecognitionDatabase::~RecognitionDatabase()
{
    // saveConfig() called from Private destructor
}

RecognitionDatabase RecognitionDatabase::addDatabase(const QString& configurationPath)
{
    QExplicitlySharedDataPointer<Private> d = static_d->database(configurationPath);
    return RecognitionDatabase(d);
}

bool RecognitionDatabase::isNull() const
{
    return !d;
}

QList<Identity> RecognitionDatabase::allIdentities() const
{
    if (!d || !d->dbAvailable)
        return QList<Identity>();

    QMutexLocker lock(&d->mutex);

    return (d->identityCache.values());
}

Identity RecognitionDatabase::identity(int id) const
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    return (d->identityCache.value(id));
}

// Takes care that there may be multiple values of attribute in identity's attributes
bool RecognitionDatabase::Private::identityContains(const Identity& identity, const QString& attribute, const QString& value) const
{
    QMap<QString, QString>::const_iterator it = identity.attributes.find(attribute);

    for (; it != identity.attributes.end() && it.key() == attribute; ++it)
    {
        if (it.value() == value)
        {
            return true;
        }
    }

    return false;
}

Identity RecognitionDatabase::Private::findByAttribute(const QString& attribute, const QString& value) const
{
    foreach (const Identity& identity, identityCache)
    {
        if (identityContains(identity, attribute, value))
        {
            return identity;
        }
    }

    return Identity();
}

// Takes care that there may be multiple values of attribute in valueMap
Identity RecognitionDatabase::Private::findByAttributes(const QString& attribute, const QMap<QString, QString>& valueMap) const
{
    QMap<QString, QString>::const_iterator it = valueMap.find(attribute);

    for (; it != valueMap.end() && it.key() == attribute; ++it)
    {
        foreach (const Identity& identity, identityCache)
        {
            if (identityContains(identity, attribute, it.value()))
            {
                return identity;
            }
        }
    }

    return Identity();
}

Identity RecognitionDatabase::findIdentity(const QString& attribute, const QString& value) const
{
    if (!d || !d->dbAvailable || attribute.isEmpty())
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    return (d->findByAttribute(attribute, value));
}

Identity RecognitionDatabase::findIdentity(const QMap<QString, QString>& attributes) const
{
    if (!d || !d->dbAvailable || attributes.isEmpty())
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    Identity match;

    // First and foremost, UUID
    QString uuid = attributes.value("uuid");
    match        = d->findByAttribute("uuid", uuid);

    if (!match.isNull())
    {
        return match;
    }

    // A negative UUID match, with a given UUID, precludes any further search
    if (!uuid.isNull())
    {
        return Identity();
    }

    // full name
    match = d->findByAttributes("fullName", attributes);

    if (!match.isNull())
    {
        return match;
    }

    // name
    match = d->findByAttributes("name", attributes);

    if (!match.isNull())
    {
        return match;
    }

    QMap<QString, QString>::const_iterator it;

    for (it = attributes.begin(); it != attributes.end(); ++it)
    {
        if (it.key() == "uuid" || it.key() == "fullName" || it.key() == "name")
        {
            continue;
        }

        match = d->findByAttribute(it.key(), it.value());

        if (!match.isNull())
        {
            return match;
        }
    }

    return Identity();
}

Identity RecognitionDatabase::addIdentity(const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    if (attributes.contains("uuid"))
    {
        Identity matchByUuid = findIdentity("uuid", attributes.value("uuid"));

        if (!matchByUuid.isNull())
        {
            // This situation is not well defined.

            kDebug() << "Called addIdentity with a given UUID, and there is such a UUID already in the database."
                     << "The existing identity is returned without adjusting properties!";

            return matchByUuid;
        }
    }

    Identity identity;
    {
        DatabaseOperationGroup group(d->db);
        int id                      = DatabaseAccess(d->db).db()->addIdentity();
        identity.id                 = id;
        identity.attributes         = attributes;
        identity.attributes["uuid"] = QUuid::createUuid().toString();
        DatabaseAccess(d->db).db()->updateIdentity(identity);
    }

    d->identityCache[identity.id] = identity;

    return identity;
}

void RecognitionDatabase::addIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        it->attributes.unite(attributes);
        DatabaseAccess(d->db).db()->updateIdentity(*it);
    }
}

void RecognitionDatabase::addIdentityAttribute(int id, const QString& attribute, const QString& value)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        it->attributes.insertMulti(attribute, value);
        DatabaseAccess(d->db).db()->updateIdentity(*it);
    }
}

void RecognitionDatabase::setIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
            return;
    }

    QMutexLocker lock(&d->mutex);
    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        it->attributes = attributes;
        DatabaseAccess(d->db).db()->updateIdentity(*it);
    }
}

QString RecognitionDatabase::backendIdentifier() const
{
    return QString("opentld");
}

void RecognitionDatabase::Private::applyParameters()
{
    if (recognizerConst())
    {
        for (QVariantMap::const_iterator it = parameters.constBegin(); it != parameters.constEnd(); ++it)
        {
            if (it.key() == "threshold" || it.key() == "accuracy")
            {
                recognizer()->setThreshold(it.value().toFloat());
            }
        }
    }
}

void RecognitionDatabase::setParameter(const QString& parameter, const QVariant& value)
{
    if (!d || !d->dbAvailable)
    {
            return;
    }

    QMutexLocker lock(&d->mutex);

    d->parameters.insert(parameter, value);
    d->applyParameters();
}

void RecognitionDatabase::setParameters(const QVariantMap& parameters)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    for (QVariantMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        d->parameters.insert(it.key(), it.value());
    }

    d->applyParameters();
}

QVariantMap RecognitionDatabase::parameters() const
{
    if (!d || !d->dbAvailable)
    {
        return QVariantMap();
    }

    QMutexLocker lock(&d->mutex);

    return d->parameters;
}

int RecognitionDatabase::recommendedImageSize(const QSize& availableSize) const
{
    // hardcoded for now, change when we know better.
    Q_UNUSED(availableSize)

    return 256;
}

Identity RecognitionDatabase::recognizeFace(const QImage& image)
{
    QList<Identity> result = recognizeFaces(QList<QImage>() << image);
    
    if (result.isEmpty())
        return Identity();

    return result.first();
}

QList<Identity> RecognitionDatabase::recognizeFaces(const QList<QImage>& images)
{
    QListImageListProvider provider(images);

    return recognizeFaces(&provider);
}

cv::Mat RecognitionDatabase::Private::preprocessingChain(const QImage& image)
{
    try
    {
        cv::Mat cvImage = recognizer()->prepareForRecognition(image);
        //cvImage         = aligner()->align(cvImage);
        //TanTriggsPreprocessor preprocessor;
        //cvImage         = preprocessor.preprocess(cvImage);
        return cvImage;
    }
    catch (cv::Exception& e)
    {
        kError() << "cv::Exception:" << e.what();
        return cv::Mat();
    }
    catch(...)
    {
        kError() << "Default exception from OpenCV";
        return cv::Mat();
    }
}

QList<Identity> RecognitionDatabase::recognizeFaces(ImageListProvider* const images)
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    QMutexLocker lock(&d->mutex);

    QList<Identity> result;

    for (; !images->atEnd(); images->proceed())
    {
        int id = -1;

        try
        {
            id = d->recognizer()->recognize(d->preprocessingChain(images->image()));
        }
        catch (cv::Exception& e)
        {
            kError() << "cv::Exception:" << e.what();
        }
        catch(...)
        {
            kError() << "Default exception from OpenCV";
        }

        if (id == -1)
        {
            result << Identity();
        }
        else
        {
            result << d->identityCache.value(id);
        }
    }

    return result;
}

RecognitionDatabase::TrainingCostHint RecognitionDatabase::trainingCostHint() const
{
    return TrainingIsCheap;
}

void RecognitionDatabase::train(const Identity& identityToBeTrained, TrainingDataProvider* const data,
                                const QString& trainingContext)
{
    train(QList<Identity>() << identityToBeTrained, data, trainingContext);
}

/// Training where the train method takes one identity and one image
template <class Recognizer>
static void trainSingle(Recognizer* const r, const Identity& identity, TrainingDataProvider* const data,
                        const QString& trainingContext, RecognitionDatabase::Private* const d)
{
    ImageListProvider* const images = data->newImages(identity);

    kDebug() << "Training" << images->size() << "images for identity" << identity.id;

    for (; !images->atEnd(); images->proceed())
    {
        try
        {
            r->train(identity.id, d->preprocessingChain(images->image()), trainingContext);
        }
        catch (cv::Exception& e)
        {
            kError() << "cv::Exception:" << e.what();
        }
        catch(...)
        {
            kError() << "Default exception from OpenCV";
        }
    }
}

/// Training where the train method takes a list of identities and images,
/// and updating per-identity is non-inferior to updating all at once.
template <class Recognizer>
static void trainIdentityBatch(Recognizer* const r, const QList<Identity>& identitiesToBeTrained,
                               TrainingDataProvider* const data, const QString& trainingContext,
                               RecognitionDatabase::Private* const d)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        std::vector<int>     labels;
        std::vector<cv::Mat> images;

        ImageListProvider* const imageList = data->newImages(identity);
        images.reserve(imageList->size());

        for (; !imageList->atEnd(); imageList->proceed())
        {
            try
            {
                cv::Mat cvImage = d->preprocessingChain(imageList->image());

                labels.push_back(identity.id);
                images.push_back(cvImage);
            }
            catch (cv::Exception& e)
            {
                kError() << "cv::Exception preparing image for LBPH:" << e.what();
            }
            catch(...)
            {
                kError() << "Default exception from OpenCV";
            }
        }

        kDebug() << "Training" << images.size() << "images for identity" << identity.id;

        try
        {
            r->train(images, labels, trainingContext);
        }
        catch (cv::Exception& e)
        {
            kError() << "cv::Exception training LBPH:" << e.what();
        }
        catch(...)
        {
            kError() << "Default exception from OpenCV";
        }
    }
}

void RecognitionDatabase::Private::train(OpenCVLBPHFaceRecognizer* const r, const QList<Identity>& identitiesToBeTrained,
                                         TrainingDataProvider* const data, const QString& trainingContext)
{
    trainIdentityBatch(r, identitiesToBeTrained, data, trainingContext, this);
}

void RecognitionDatabase::train(const QList<Identity>& identitiesToBeTrained, TrainingDataProvider* const data,
                                const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
            return;
    }

    QMutexLocker lock(&d->mutex);

    d->train(d->recognizer(), identitiesToBeTrained, data, trainingContext);
}

void RecognitionDatabase::Private::clear(OpenCVLBPHFaceRecognizer* const, const QList<int>& idsToClear, const QString& trainingContext)
{
    // force later reload
    delete opencvlbph;
    opencvlbph = 0;

    if (idsToClear.isEmpty())
    {
        DatabaseAccess(db).db()->clearLBPHTraining(trainingContext);
    }
    else
    {
        DatabaseAccess(db).db()->clearLBPHTraining(idsToClear, trainingContext);
    }
}

void RecognitionDatabase::clearAllTraining(const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    d->clear(d->recognizer(), QList<int>(), trainingContext);
}

void RecognitionDatabase::clearTraining(const QList<Identity>& identitiesToClean, const QString& trainingContext)
{
    if (!d || !d->dbAvailable || identitiesToClean.isEmpty())
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    QList<int>   ids;

    foreach (const Identity& id, identitiesToClean)
    {
        ids << id.id;
    }

    d->clear(d->recognizer(), ids, trainingContext);
}

void RecognitionDatabase::deleteIdentity(const Identity& identityToBeDeleted)
{
    if (!d || !d->dbAvailable || identityToBeDeleted.isNull())
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    DatabaseAccess(d->db).db()->deleteIdentity(identityToBeDeleted.id);
    d->identityCache.remove(identityToBeDeleted.id);
}

// --- Runtime version info static methods (declared in version.h) --------------------------------------------------

QString LibOpenCVVersion()
{
    return QString("%1").arg(CV_VERSION);
}

QString version()
{
    return QString(kface_version);
}

} // namespace KFaceIface
