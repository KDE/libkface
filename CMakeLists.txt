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

INCLUDE(MacroOpenCV)
DETECT_OPENCV(2.4.9 core highgui objdetect contrib legacy imgproc)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

IF (OpenCV_FOUND)

    # =======================================================
    # Information to update before to release this library.

    # Library version history:
    # API      ABI
    # 0.1.0 => 0.1.0     (During digiKam GoSC2010)
    # 2.0.0 => 1.0.0     (Included into digiKam 2.0.0 Software Collection)
    # 3.0.0 => 2.0.0     (Including LBPH algorithm for face recognition)
    # 3.1.0 => 2.0.0     (Big private code review. No BC broken).
    # 3.2.0 => 2.0.0     (Added the deleteIdentity function to the RecognitionDatabase class)
    # 3.3.0 => 2.0.0     (Added a "simple" training method using image data directly for a single image)
    # 3.4.0 => 2.0.0     (Added a "simple" training method using image data directly for an image list)
    # 3.5.0 => 3.0.0     (Added d private internal container to reduce binary uncompatibility with Identity class)

    # Library API version
    SET(KFACE_LIB_MAJOR_VERSION "3")
    SET(KFACE_LIB_MINOR_VERSION "5")
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
    SET(KFACE_LIB_SO_CUR_VERSION "3")
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
    ENDIF()

    IF(NOT WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libkface.pc.cmake.in  ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libkface.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkface.lsm)
        INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkface.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
    ENDIF()

    # Install find cmake script to the system for client applications.

    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindKface.cmake
            DESTINATION ${DATA_INSTALL_DIR}/cmake/modules)

ELSE()

    MESSAGE(FATAL_ERROR "LibKface cannot be compiled.")

ENDIF()
