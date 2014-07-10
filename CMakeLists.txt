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

PROJECT(libkface)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "Starting CMake configuration for: libkface")

FIND_PACKAGE(Qt4 4.6.0 REQUIRED)
FIND_PACKAGE(KDE4 REQUIRED)

ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(MacroOptionalFindPackage)
INCLUDE(FindPackageHandleStandardArgs)

ADD_DEFINITIONS (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
INCLUDE_DIRECTORIES (${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES})

SET(LIBKFACE_AREA_CODE_GENERAL 51005)
ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${LIBKFACE_AREA_CODE_GENERAL})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# BEGIN SPECIAL OPENCV HANDLING

# On some systems, OpenCV can be found using FIND_PACKAGE(OpenCV), for other systems
# we ship our own version of FindOpenCV.cmake. But that does not work on all systems.
# Therefore, first try finding OpenCV using FIND_PACKAGE(OpenCV), and if that fails,
# add our FindOpenCV.cmake to the search path and search again.

MESSAGE(STATUS "First try at finding OpenCV...")
FIND_PACKAGE(OpenCV)

IF (NOT OpenCV_LIBRARIES AND NOT OpenCV_LIBS)
  MESSAGE(STATUS "Could not find OpenCV normally, trying internal FindOpenCV.cmake")
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules_opencv)
  FIND_PACKAGE(OpenCV REQUIRED)
ELSE (NOT OpenCV_LIBRARIES AND NOT OpenCV_LIBS)
  MESSAGE(STATUS "Great, found OpenCV on the first try.")
ENDIF (NOT OpenCV_LIBRARIES AND NOT OpenCV_LIBS)

MESSAGE(STATUS "OpenCV Root directory is: ${OpenCV_DIR}")

# check OpenCV version

# Libkface can be compiled with OpenCV 2.4.3, but at least 2.4.5 is suitbale without main bugs for Faces Recognition.
SET(OPENCV_MIN_VERSION "2.4.9")

IF (OpenCV_VERSION)

    MESSAGE(STATUS "OpenCV: Found version ${OpenCV_VERSION} (required: ${OPENCV_MIN_VERSION})")
    IF (${OpenCV_VERSION} VERSION_LESS ${OPENCV_MIN_VERSION})
        MESSAGE(WARNING "OpenCV: Version is too old.")
        SET(OpenCV_FOUND FALSE)
    ENDIF (${OpenCV_VERSION} VERSION_LESS ${OPENCV_MIN_VERSION})

ELSE (OpenCV_VERSION)

    MESSAGE(WARNING "OpenCV: Version information not found, your version is probably too old.")
    SET(OpenCV_FOUND FALSE)

ENDIF (OpenCV_VERSION)

# there are two versions of FindOpenCV.cmake in the wild, one defining
# OpenCV_LIBRARIES, the other defining OpenCV_LIBS. Make sure we handle
# both cases.
IF (NOT OpenCV_LIBRARIES)
  SET(OpenCV_LIBRARIES ${OpenCV_LIBS})
ENDIF (NOT OpenCV_LIBRARIES)

# same story with OpenCV_INCLUDE_DIRS and OpenCV_INCLUDE_DIR:
IF (NOT OpenCV_INCLUDE_DIRS)
  SET(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
ENDIF (NOT OpenCV_INCLUDE_DIRS)

# END SPECIAL OPENCV HANDLING

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

IF (OpenCV_FOUND)

# =======================================================
# Information to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0     (During digiKam GoSC2010)
# 2.0.0 => 1.0.0     (Included into digiKam 2.0.0 Software Collection)
# 3.0.0 => 2.0.0     (Including OpenTLD for face recognition)
# 3.1.0 => 2.0.0     (Big private code review. No BC broken).
# 3.2.0 => 2.0.0     (Added the deleteIdentity function to the RecognitionDatabase class)

# Library API version
SET(KFACE_LIB_MAJOR_VERSION "3")
SET(KFACE_LIB_MINOR_VERSION "2")
SET(KFACE_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KFACE_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(KFACE_LIB_SO_CUR_VERSION "2")
SET(KFACE_LIB_SO_REV_VERSION "0")
SET(KFACE_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

SET(KFACE_LIB_VERSION_STRING "${KFACE_LIB_MAJOR_VERSION}.${KFACE_LIB_MINOR_VERSION}.${KFACE_LIB_PATCH_VERSION}${KFACE_LIB_SUFFIX_VERSION}")
SET(KFACE_LIB_VERSION_ID "0x0${KFACE_LIB_MAJOR_VERSION}0${KFACE_LIB_MINOR_VERSION}0${KFACE_LIB_PATCH_VERSION}")
SET(KFACE_LIB_SO_VERSION_STRING "${KFACE_LIB_SO_CUR_VERSION}.${KFACE_LIB_SO_REV_VERSION}.${KFACE_LIB_SO_AGE_VERSION}")

# =======================================================

SUBDIRS(data)
SUBDIRS(libkface)

IF(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
ENDIF(KDE4_BUILD_TESTS)

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libkface.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libkface.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkface.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
ENDIF(NOT WIN32)

# Install find cmake script to the system for client applications.

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindKface.cmake
        DESTINATION ${DATA_INSTALL_DIR}/cmake/modules)

ELSE(OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "LibKface cannot be compiled.")
ENDIF(OpenCV_FOUND)
