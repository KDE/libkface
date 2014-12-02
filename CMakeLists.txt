#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(libkface)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: ${PROJECT_NAME}")

set(CMAKE_MIN_VERSION   "2.8.12")
set(ECM_MIN_VERSION     "1.1.0")
set(KF5_VERSION         "5.1.0")
set(REQUIRED_QT_VERSION "5.2.0")
set(OPENCV_MIN_VERSION  "2.4.9")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

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
# 5.0.0 => 10.0.0    (Released with KDE 5.x)

# Library API version
set(KFACE_LIB_MAJOR_VERSION "5")
set(KFACE_LIB_MINOR_VERSION "0")
set(KFACE_LIB_PATCH_VERSION "0")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(KFACE_LIB_SO_CUR_VERSION "10")
set(KFACE_LIB_SO_REV_VERSION "0")
set(KFACE_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

set(LIBKFACE_LIB_VERSION "${KFACE_LIB_MAJOR_VERSION}.${KFACE_LIB_MINOR_VERSION}.${KFACE_LIB_PATCH_VERSION}")
set(LIBKFACE_SO_VERSION  "${KFACE_LIB_SO_CUR_VERSION}.${KFACE_LIB_SO_REV_VERSION}.${KFACE_LIB_SO_AGE_VERSION}")

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkface_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(GenerateExportHeader)
include(FeatureSummary)

############## Find Packages ###################

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS
             Core
             Widgets
             Sql
             Xml
             Gui
)

find_package(KF5 ${KF5_VERSION} REQUIRED COMPONENTS
             I18n
)

# OpenCV detection
include(MacroOpenCV)
DETECT_OPENCV(${OPENCV_MIN_VERSION} core highgui objdetect contrib legacy imgproc)
include_directories(${OpenCV_INCLUDE_DIRS})

if(OpenCV_FOUND)

    ecm_setup_version(${LIBKFACE_LIB_VERSION}
                      VARIABLE_PREFIX      KFACE
                      VERSION_HEADER       "${CMAKE_CURRENT_BINARY_DIR}/libkface_version.h"
                      PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5KFaceConfigVersion.cmake"
                      SOVERSION            ${LIBKFACE_SO_VERSION}
    )

    ############## Targets #########################

    add_definitions(-DTRANSLATION_DOMAIN=\"libkface\")

    add_subdirectory(data)
    add_subdirectory(src)

    if (BUILD_TESTING)
        add_subdirectory(tests)
    endif()

    ############## CMake Config Files ##############

    message("${CMAKECONFIG_INSTALL_DIR}")
    set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5KFace")

    ecm_configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/KF5KFaceConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/KF5KFaceConfig.cmake"
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/KF5KFaceConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/KF5KFaceConfigVersion.cmake"
            DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
            COMPONENT   Devel
    )

    install(EXPORT      KF5KFaceTargets
            DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
            FILE        KF5KFaceTargets.cmake
            NAMESPACE   KF5::
    )

else()

    message(FATAL_ERROR "LibKface cannot be compiled.")

endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
