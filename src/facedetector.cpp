/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date  2010-09-02
 * @brief A convenience class for a standalone face detector
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010-2015 by Gilles Caulier
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
#include "opencvfacedetector.h"

// Qt includes

#include <QSharedData>
#include <QStandardPaths>

// Local includes

#include "libkface_debug.h"
#include "facedetector.h"

namespace KFaceIface
{

class FaceDetector::Private : public QSharedData
{
public:

    Private()
        : m_backend(0)
    {
    }

    ~Private()
    {
        delete m_backend;
    }

    OpenCVFaceDetector* backend()
    {
        if (!m_backend)
        {
            QStringList cascadeDirs;
            // First try : typically work everywhere if packagers don't drop libkface shared data files.
            cascadeDirs << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,  QString::fromLatin1("libkface/haarcascades"),  QStandardPaths::LocateDirectory);

            // Second try to find OpenCV shared files. Work only under Linux and OSX. OpenCV do not install XML files under Windows (checked with OpenCV 2.4.9)
            // Try under both CamelCase *and* lowercase directories, as both seems to be used.
            cascadeDirs << QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation, QString::fromLatin1("../OpenCV/haarcascades"), QStandardPaths::LocateDirectory);
            cascadeDirs << QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation, QString::fromLatin1("../opencv/haarcascades"), QStandardPaths::LocateDirectory);

            // Last try to find OpenCV shared files, using cmake env variables.
            cascadeDirs << QString::fromLatin1("%1/haarcascades").arg(QString::fromLatin1(OPENCV_ROOT_PATH));

            qCDebug(LIBKFACE_LOG) << "Try to find OpenCV Haar Cascade files in these directories: " << cascadeDirs;

            m_backend = new OpenCVFaceDetector(cascadeDirs);
            applyParameters();
        }

        return m_backend;
    }

    const OpenCVFaceDetector* constBackend() const
    {
        return m_backend;
    }

    void applyParameters()
    {
        if (!m_backend)
        {
            return;
        }
        for (QVariantMap::const_iterator it = m_parameters.constBegin(); it != m_parameters.constEnd(); ++it)
        {
            if (it.key() == QString::fromLatin1("accuracy"))
            {
                backend()->setAccuracy(it.value().toDouble());
            }
            else if (it.key() == QString::fromLatin1("speed"))
            {
                backend()->setAccuracy(1.0 - it.value().toDouble());
            }
            else if (it.key() == QString::fromLatin1("specificity"))
            {
                backend()->setSpecificity(it.value().toDouble());
            }
            else if (it.key() == QString::fromLatin1("sensitivity"))
            {
                backend()->setSpecificity(1.0 - it.value().toDouble());
            }
        }
    }

public:

    QVariantMap         m_parameters;

private:

    OpenCVFaceDetector* m_backend;
};

// ---------------------------------------------------------------------------------

FaceDetector::FaceDetector()
    : d(new Private)
{
}

FaceDetector::FaceDetector(const FaceDetector& other)
{
    d = other.d;
}

FaceDetector& FaceDetector::operator=(const KFaceIface::FaceDetector& other)
{
    d = other.d;
    return *this;
}

FaceDetector::~FaceDetector()
{
}

QString FaceDetector::backendIdentifier() const
{
    return QString::fromLatin1("OpenCV Cascades");
}

QList<QRectF> FaceDetector::detectFaces(const QImage& image, const QSize& originalSize)
{
    QList<QRectF> result;

    try
    {
        cv::Size cvOriginalSize;

        if (originalSize.isValid())
        {
            cvOriginalSize = cv::Size(originalSize.width(), originalSize.height());
        }
        else
        {
            cvOriginalSize = cv::Size(image.width(), image.height());
        }

        cv::Mat cvImage       = d->backend()->prepareForDetection(image);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, cvOriginalSize);
        result                = toRelativeRects(absRects, QSize(cvImage.cols, cvImage.rows));

    }
    catch (cv::Exception& e)
    {
        qCCritical(LIBKFACE_LOG) << "cv::Exception:" << e.what();
    }
    catch(...)
    {
        qCCritical(LIBKFACE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

void FaceDetector::setParameter(const QString& parameter, const QVariant& value)
{
    d->m_parameters.insert(parameter, value);
    d->applyParameters();
}

void FaceDetector::setParameters(const QVariantMap& parameters)
{
    for (QVariantMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        d->m_parameters.insert(it.key(), it.value());
    }

    d->applyParameters();
}

QVariantMap FaceDetector::parameters() const
{
    return d->m_parameters;
}

int FaceDetector::recommendedImageSize(const QSize& availableSize) const
{
    Q_UNUSED(availableSize);
    return OpenCVFaceDetector::recommendedImageSizeForDetection();
}

// -- Static methods -------------------------------------------------------------

QRectF FaceDetector::toRelativeRect(const QRect& abs, const QSize& s)
{
    if (s.isEmpty())
    {
        return QRectF();
    }

    return QRectF(qreal(abs.x())      / qreal(s.width()),
                  qreal(abs.y())      / qreal(s.height()),
                  qreal(abs.width())  / qreal(s.width()),
                  qreal(abs.height()) / qreal(s.height()));
}

QRect FaceDetector::toAbsoluteRect(const QRectF& rel, const QSize& s)
{
    return QRectF(rel.x()      * s.width(),
                  rel.y()      * s.height(),
                  rel.width()  * s.width(),
                  rel.height() * s.height()).toRect();
}

QList<QRectF> FaceDetector::toRelativeRects(const QList<QRect>& absoluteRects, const QSize& size)
{
    QList<QRectF> result;

    foreach (const QRect& r, absoluteRects)
    {
        result << toRelativeRect(r, size);
    }

    return result;
}

QList<QRect> FaceDetector::toAbsoluteRects(const QList<QRectF>& relativeRects, const QSize& size)
{
    QList<QRect> result;

    foreach (const QRectF& r, relativeRects)
    {
        result << toAbsoluteRect(r, size);
    }

    return result;
}

} // namespace KFaceIface
