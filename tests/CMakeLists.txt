#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# -----------------------------------------------------------------------------

set(detect_SRCS detect.cpp)
add_executable(detect ${detect_SRCS})
target_link_libraries(detect KF5KFace Qt5::Core Qt5::Gui ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(recognize_SRCS recognize.cpp)
add_executable(recognize ${recognize_SRCS})
target_link_libraries(recognize KF5KFace Qt5::Core Qt5::Gui ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(align_SRCS align.cpp
               # not exported by libkface
               ../src/alignment-congealing/funnelreal.cpp
               ../src/libkface_debug.cpp
)
add_executable(align ${align_SRCS})
target_link_libraries(align KF5KFace Qt5::Core Qt5::Gui ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(preprocess_SRCS preprocess.cpp)
add_executable(preprocess ${preprocess_SRCS})
target_link_libraries(preprocess KF5KFace Qt5::Core Qt5::Gui ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(traindb_SRCS traindb.cpp)
add_executable(traindb ${traindb_SRCS})
target_link_libraries(traindb KF5KFace Qt5::Core Qt5::Gui ${OpenCV_LIBRARIES})

# -----------------------------------------------------------------------------

set(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/faceitem.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/button.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/marquee.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/gui/fancyrect.cpp
)

ki18n_wrap_ui(kfacegui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui)
add_executable(kfacegui ${kfacegui_SRCS})

target_link_libraries(kfacegui KF5KFace
                               Qt5::Core
                               Qt5::Gui
                               ${OpenCV_LIBRARIES}
)
