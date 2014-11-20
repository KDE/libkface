#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/database
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition-opencv-lbph
                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment-congealing
#                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment-flandmark
#                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing-tantriggs
)

remove_definitions(-DQT_NO_STL)

set(kface_LIB_SRCS facedetector.cpp
                   libkface_debug.cpp
                   identity.cpp
                   dataproviders.cpp
                   recognitiondatabase.cpp
                   detection/opencvfacedetector.cpp
                   recognition-opencv-lbph/lbphfacemodel.cpp
                   recognition-opencv-lbph/opencvlbphfacerecognizer.cpp
                   recognition-opencv-lbph/facerec_borrowed.cpp
                   alignment-congealing/funnelreal.cpp
#                   alignment-flandmark/flandmarkaligner.cpp
#                   preprocessing-tantriggs/tantriggspreprocessor.cpp
)

set(kface_database_LIB_SRCS database/databaseaccess.cpp
                            database/databasecorebackend.cpp
                            database/databaseparameters.cpp
                            database/dbactiontype.cpp
                            database/schemaupdater.cpp
                            database/databaseconfigelement.cpp
                            database/databaseoperationgroup.cpp
                            database/trainingdb.cpp
                            database/sqlquery.cpp
                            database/dbcontainers.cpp
)

ecm_generate_headers(kface_CamelCase_HEADERS

                     HEADER_NAMES
                     FaceDetector
                     RecognitionDatabase
                     Identity
                     DataProviders

                     PREFIX           KFACE
                     REQUIRED_HEADERS kface_HEADERS
)

add_library(KF5KFace SHARED ${kface_LIB_SRCS} ${kface_database_LIB_SRCS})
add_library(KF5::KFace ALIAS KF5KFace)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in   ${CMAKE_CURRENT_BINARY_DIR}/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libopencv.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libopencv.h)

generate_export_header(KF5KFace BASE_NAME libkface EXPORT_MACRO_NAME LIBKFACE_EXPORT)

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
if(NOT DIGIKAMSC_COMPILE_LIBKFACE)

    install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/libkface_export.h
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KFACE/kface
            COMPONENT   Devel
    )

    install(FILES       ${kface_CamelCase_HEADERS}
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KFACE/kface
            COMPONENT   Devel
    )

    install(FILES       ${kface_HEADERS}
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KFACE/kface
            COMPONENT   Devel
    )

    install(FILES       "${CMAKE_BINARY_DIR}/libkface_version.h"
            DESTINATION "${KF5_INCLUDE_INSTALL_DIR}"
            COMPONENT   Devel
    )

endif()

target_include_directories(KF5KFace INTERFACE 
                           "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KFACE;${KF5_INCLUDE_INSTALL_DIR}/KFACE/kface>"
)

target_link_libraries(KF5KFace
                      PRIVATE
                      ${OpenCV_LIBRARIES}
                      PUBLIC
                      KF5::KDELibs4Support
                      Qt5::Gui
                      Qt5::Xml
                      Qt5::Sql
)

install(TARGETS KF5KFace
        EXPORT  KF5KFaceTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KF5KFace    PROPERTIES
                      VERSION     ${KFACE_VERSION_STRING}
                      SOVERSION   ${KFACE_SOVERSION}
                      EXPORT_NAME KFace
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/database/dbconfig.xml
        DESTINATION ${DATA_INSTALL_DIR}/libkface/database )
