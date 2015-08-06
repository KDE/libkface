#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

cmake_minimum_required(VERSION 2.8.9)
project(libkface)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: libkface")

find_package(Qt4 4.6.0 REQUIRED)
find_package(KDE4 REQUIRED)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(MacroOptionalFindPackage)
include(FindPackageHandleStandardArgs)

add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES})

set(LIBKFACE_AREA_CODE_GENERAL 51005)
add_definitions(-DKDE_DEFAULT_DEBUG_AREA=${LIBKFACE_AREA_CODE_GENERAL})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(MacroOpenCV)
DETECT_OPENCV(2.4.9 core highgui objdetect contrib legacy imgproc)

include_directories(${OpenCV_INCLUDE_DIRS})

if(OpenCV_FOUND)

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
    set(KFACE_LIB_MAJOR_VERSION "3")
    set(KFACE_LIB_MINOR_VERSION "5")
    set(KFACE_LIB_PATCH_VERSION "0")

    # Suffix to add at end of version string. Usual values are:
    # "-git"   : alpha code unstable from git. Do not use in production
    # "-beta1" : beta1 release.
    # "-beta2" : beta2 release.
    # "-beta3" : beta3 release.
    # "-rc"    : release candidate.
    # ""       : final relase. Can be used in production.
    set(KFACE_LIB_SUFFIX_VERSION "")

    # Library ABI version used by linker.
    # For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
    set(KFACE_LIB_SO_CUR_VERSION "3")
    set(KFACE_LIB_SO_REV_VERSION "0")
    set(KFACE_LIB_SO_AGE_VERSION "0")

    # =======================================================
    # Set env. variables accordinly.

    set(KFACE_LIB_VERSION_STRING "${KFACE_LIB_MAJOR_VERSION}.${KFACE_LIB_MINOR_VERSION}.${KFACE_LIB_PATCH_VERSION}${KFACE_LIB_SUFFIX_VERSION}")
    set(KFACE_LIB_VERSION_ID "0x0${KFACE_LIB_MAJOR_VERSION}0${KFACE_LIB_MINOR_VERSION}0${KFACE_LIB_PATCH_VERSION}")
    set(KFACE_LIB_SO_VERSION_STRING "${KFACE_LIB_SO_CUR_VERSION}.${KFACE_LIB_SO_REV_VERSION}.${KFACE_LIB_SO_AGE_VERSION}")

    # =======================================================

    subdirs(data)
    subdirs(libkface)

    if(KDE4_BUILD_TESTS)
        MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
    endif()

    # Install find cmake script to the system for client applications.

    include(CMakePackageConfigHelpers)

    CONFIGURE_PACKAGE_CONFIG_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/kface-config.cmake.in kface-config.cmake
                                  INSTALL_DESTINATION "${LIB_INSTALL_DIR}/cmake/Kface-${KFACE_LIB_VERSION_STRING}"
                                  PATH_VARS INCLUDE_INSTALL_DIR
                                  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    WRITE_BASIC_PACKAGE_VERSION_FILE(kface-config-version.cmake
                                     VERSION "${KFACE_LIB_MAJOR_VERSION}.${KFACE_LIB_MINOR_VERSION}.${KFACE_LIB_PATCH_VERSION}"
                                     COMPATIBILITY SameMajorVersion)
    install(EXPORT Kface
            DESTINATION "${LIB_INSTALL_DIR}/cmake/Kface-${KFACE_LIB_VERSION_STRING}"
            NAMESPACE Kface::
            FILE Kface-export.cmake)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/kface-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/kface-config-version.cmake
            DESTINATION
            "${LIB_INSTALL_DIR}/cmake/Kface-${KFACE_LIB_VERSION_STRING}")

else()

    message(FATAL_ERROR "LibKface cannot be compiled.")

endif()
