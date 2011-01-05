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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkface)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)

SET(detect_SRCS detect.cpp)
KDE4_ADD_EXECUTABLE(detect ${detect_SRCS})
TARGET_LINK_LIBRARIES(detect kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

# -----------------------------------------------------------------------------

SET(libfaceGUI_SRCS gui/main.cpp
                    gui/mainwindow.cpp
                    gui/faceitem.cpp
                    gui/button.cpp
                    gui/marquee.cpp
                    gui/fancyrect.cpp
   )

KDE4_ADD_UI_FILES(libfaceGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui)
KDE4_ADD_EXECUTABLE(libfaceGUI ${libfaceGUI_SRCS})

TARGET_LINK_LIBRARIES(libfaceGUI kface ${KDE4_KDECORE_LIBS}
                                       ${KDE4_KDEUI_LIBS}
                                       ${QT_QTCORE_LIBRARY}
                                       ${QT_QTGUI_LIBRARY}
                     )
