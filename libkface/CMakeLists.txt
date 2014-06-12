# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2010-06-16
# @brief  LibFace library interface for KDE
#
# @author Copyright (C) 2010-2014 by Gilles Caulier
#         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

# To handle OpenCV C++ exceptions.
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

ADD_DEFINITIONS(${LIBFACE_DEFINITIONS})

REMOVE_DEFINITIONS(-DQT_NO_STL)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OpenTLD/src/libopentld/tld
                    ${CMAKE_CURRENT_SOURCE_DIR}/database
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition-opentld
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition-opencv-lbph
                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment-congealing
                   )

SET(kface_LIB_SRCS facedetector.cpp
                   detection/opencvfacedetector.cpp
                   recognition-opencv-lbph/lbphfacemodel.cpp
                   recognition-opencv-lbph/opencvlbphfacerecognizer.cpp
                   recognition-opencv-lbph/facerec_borrowed.cpp
                   alignment-congealing/funnelreal.cpp
                   #alignment-flandmark/liblbp.cpp
                   #alignment-flandmark/flandmark_detector.cpp
                   preprocessing-tantriggs/tantriggspreprocessor.cpp
                   recognitiondatabase.cpp
   )

SET(kface_database_LIB_SRCS database/databaseaccess.cpp
                            database/databasecorebackend.cpp
                            database/databaseparameters.cpp
                            database/dbactiontype.cpp
                            database/schemaupdater.cpp
                            database/databaseconfigelement.cpp
                            database/databaseoperationgroup.cpp
                            database/trainingdb.cpp
                            database/sqlquery.cpp
                            database/dbcontainers.cpp
   )

KDE4_ADD_LIBRARY(kface SHARED ${kface_LIB_SRCS} ${face_LIB_SRCS} ${kface_database_LIB_SRCS})

TARGET_LINK_LIBRARIES(kface ${KDE4_KDECORE_LIBS}
                            ${KDE4_KDEUI_LIBS}

                            ${QT_QTGUI_LIBRARY}
                            ${QT_QTXML_LIBRARY}
                            ${QT_QTSQL_LIBRARY}

                            ${OpenCV_LIBRARIES}

                            ${LIBFACE_LIBRARIES}
                      )

SET_TARGET_PROPERTIES(kface PROPERTIES VERSION ${KFACE_LIB_SO_VERSION_STRING}
                            SOVERSION ${KFACE_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kface ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/database/dbconfig.xml
        DESTINATION ${DATA_INSTALL_DIR}/libkface/database )

INSTALL(FILES facedetector.h
              recognitiondatabase.h
              identity.h
              dataproviders.h
              libkface_export.h
              ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
       )
