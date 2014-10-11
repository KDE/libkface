# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2010-06-16
# @brief  LibKFace library interface for KDE
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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../libkface)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkface)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)

SET(detect_SRCS detect.cpp)
KDE4_ADD_EXECUTABLE(detect ${detect_SRCS})
TARGET_LINK_LIBRARIES(detect kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

SET(recognize_SRCS recognize.cpp)
KDE4_ADD_EXECUTABLE(recognize ${recognize_SRCS})
TARGET_LINK_LIBRARIES(recognize kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

SET(align_SRCS align.cpp)
KDE4_ADD_EXECUTABLE(align ${align_SRCS})
TARGET_LINK_LIBRARIES(align kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

SET(preprocess_SRCS preprocess.cpp)
KDE4_ADD_EXECUTABLE(preprocess ${preprocess_SRCS})
TARGET_LINK_LIBRARIES(preprocess kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

SET(traindb_SRCS traindb.cpp)
KDE4_ADD_EXECUTABLE(traindb ${traindb_SRCS})
TARGET_LINK_LIBRARIES(traindb kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

SET(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/faceitem.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/button.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/marquee.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/fancyrect.cpp
   )

KDE4_ADD_UI_FILES(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui)
KDE4_ADD_EXECUTABLE(kfacegui ${kfacegui_SRCS})

TARGET_LINK_LIBRARIES(kfacegui kface ${KDE4_KDECORE_LIBS}
                                     ${KDE4_KDEUI_LIBS}
                                     ${KDE4_KIO_LIBS}
                                     ${QT_QTCORE_LIBRARY}
                                     ${QT_QTGUI_LIBRARY}
                                     ${OpenCV_LIBRARIES}
                     )
