#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../libkface)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libkface)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(detect_SRCS detect.cpp)
KDE4_ADD_EXECUTABLE(detect ${detect_SRCS})
target_link_libraries(detect kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

set(recognize_SRCS recognize.cpp)
KDE4_ADD_EXECUTABLE(recognize ${recognize_SRCS})
target_link_libraries(recognize kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

set(align_SRCS align.cpp)
KDE4_ADD_EXECUTABLE(align ${align_SRCS})
target_link_libraries(align kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

set(preprocess_SRCS preprocess.cpp)
KDE4_ADD_EXECUTABLE(preprocess ${preprocess_SRCS})
target_link_libraries(preprocess kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

set(traindb_SRCS traindb.cpp)
KDE4_ADD_EXECUTABLE(traindb ${traindb_SRCS})
target_link_libraries(traindb kface ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/faceitem.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/button.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/marquee.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/fancyrect.cpp
   )

KDE4_ADD_UI_FILES(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui)
KDE4_ADD_EXECUTABLE(kfacegui ${kfacegui_SRCS})

target_link_libraries(kfacegui kface ${KDE4_KDECORE_LIBS}
                                     ${KDE4_KDEUI_LIBS}
                                     ${KDE4_KIO_LIBS}
                                     ${QT_QTCORE_LIBRARY}
                                     ${QT_QTGUI_LIBRARY}
                                     ${OpenCV_LIBRARIES}
                     )
