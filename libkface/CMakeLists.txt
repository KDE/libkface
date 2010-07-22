# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2010-06-16
# @brief  LibFace library interface for KDE
#
# @author Copyright (C) 2010 by Gilles Caulier
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

INCLUDE_DIRECTORIES( ${LIBFACE_INCLUDE_DIR}
                     ${OpenCV_INCLUDE_DIRS}
                   )

ADD_DEFINITIONS(${LIBFACE_DEFINITIONS})
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})
REMOVE_DEFINITIONS(-DQT_NO_STL)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(kface_LIB_SRCS database.cpp
                   kface.cpp
                   kfaceutils.cpp
                   image.cpp
                   faceitem.cpp
                   button.cpp
   )

KDE4_ADD_LIBRARY(kface SHARED ${kface_LIB_SRCS})

TARGET_LINK_LIBRARIES(kface ${KDE4_KDECORE_LIBS}
                            ${QT_QTGUI_LIBRARY}
                            ${LIBFACE_LIBRARIES}
                            ${OpenCV_LIBS}
                            )

SET_TARGET_PROPERTIES(kface PROPERTIES VERSION ${KFACE_LIB_SO_VERSION_STRING}
                            SOVERSION ${KFACE_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kface ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES database.h
              kface.h
              kfaceutils.h
              libkface_export.h
              image.h
              faceitem.h
              button.h
              ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
       )
