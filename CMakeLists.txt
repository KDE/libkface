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

PROJECT(libkface)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "Starting CMake configuration for: libkface")

FIND_PACKAGE(Qt4 4.6.0 REQUIRED)
FIND_PACKAGE(KDE4 REQUIRED)

INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(MacroOptionalFindPackage)
INCLUDE(FindPackageHandleStandardArgs)

ADD_DEFINITIONS (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
INCLUDE_DIRECTORIES (${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES})

SET(LIBKFACE_AREA_CODE_GENERAL 51005)
ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${LIBKFACE_AREA_CODE_GENERAL})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
# some users have problems with the FindOpenCV.cmake that we ship.
# specify -DKFACE_EXTERNAL_FINDOPENCV:BOOL=TRUE as an argument to cmake to use
# the FindOpenCV.cmake which is installed on your system
IF (NOT KFACE_EXTERNAL_FINDOPENCV)
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules_opencv)
ENDIF (NOT KFACE_EXTERNAL_FINDOPENCV)

FIND_PACKAGE(LibFace)
FIND_PACKAGE(OpenCV REQUIRED)

# there are two versions of FindOpenCV.cmake in the wild, one defining
# OpenCV_LIBRARIES, the other defining OpenCV_LIBS. Make sure we handle
# both cases.
IF (NOT OpenCV_LIBRARIES)
  SET(OPENCV_LIBRARIES ${OpenCV_LIBS})
ENDIF (NOT OpenCV_LIBRARIES)

# same story with OpenCV_INCLUDE_DIRS and OpenCV_INCLUDE_DIR:
IF (NOT OpenCV_INCLUDE_DIRS)
  SET(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
ENDIF (NOT OpenCV_INCLUDE_DIRS)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
IF (LIBFACE_FOUND)
  INCLUDE_DIRECTORIES(${LIBFACE_INCLUDE_DIR})
endif (LIBFACE_FOUND)

# =======================================================
# Information to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0     (During digiKam GoSC2010)
# 2.0.0 => 1.0.0     (Included into digiKam 2.0.0 Software Collection)

# Library API version
SET(KFACE_LIB_MAJOR_VERSION "2")
SET(KFACE_LIB_MINOR_VERSION "0")
SET(KFACE_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KFACE_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(KFACE_LIB_SO_CUR_VERSION "1")
SET(KFACE_LIB_SO_REV_VERSION "0")
SET(KFACE_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

SET(KFACE_LIB_VERSION_STRING "${KFACE_LIB_MAJOR_VERSION}.${KFACE_LIB_MINOR_VERSION}.${KFACE_LIB_PATCH_VERSION}${KFACE_LIB_SUFFIX_VERSION}")
SET(KFACE_LIB_VERSION_ID "0x0${KFACE_LIB_MAJOR_VERSION}0${KFACE_LIB_MINOR_VERSION}0${KFACE_LIB_PATCH_VERSION}")
SET(KFACE_LIB_SO_VERSION_STRING "${KFACE_LIB_SO_CUR_VERSION}.${KFACE_LIB_SO_REV_VERSION}.${KFACE_LIB_SO_AGE_VERSION}")

# =======================================================

IF (NOT LIBFACE_FOUND)
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libface )

    SET(face_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libface/LibFaceUtils.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/libface/FaceDetect.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/libface/Face.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/libface/Eigenfaces.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/libface/LibFace.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/libface/Haarcascades.cpp
       )

    SET(LIBFACE_LIBRARIES "")

    # NOTE: update libface version ID when code is updated from SF.net
    SET(libface_MAJOR_VERSION 0)
    SET(libface_MINOR_VERSION 1)

    SET(DEBUG_MODE 0)
    SET(LIB_TYPE STATIC)
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libface/LibFaceConfig.h.cmake"
                   "${CMAKE_CURRENT_BINARY_DIR}/libface/LibFaceConfig.h")

    MESSAGE(STATUS "External libface was not found, use internal version instead...")
ENDIF (NOT LIBFACE_FOUND)

SUBDIRS(libkface)
SUBDIRS(test)

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkface.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkface.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkface.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
ENDIF(NOT WIN32)

# Install find cmake script to the system for client applications.

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindKface.cmake
        DESTINATION ${DATA_INSTALL_DIR}/cmake/modules)
