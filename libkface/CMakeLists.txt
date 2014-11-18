#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

remove_definitions(-DQT_NO_STL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in   ${CMAKE_CURRENT_BINARY_DIR}/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libopencv.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libopencv.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/database
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition-opencv-lbph
                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment-congealing
#                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment-flandmark
#                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing-tantriggs
                   )

set(kface_LIB_SRCS facedetector.cpp
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

add_library(kface SHARED ${kface_LIB_SRCS} ${kface_database_LIB_SRCS})

generate_export_header(kface BASE_NAME kface)


target_link_libraries(kface KF5::KDELibs4Support
                            Qt5::Gui
                            Qt5::Xml
                            Qt5::Sql
                            ${OpenCV_LIBRARIES}
                      )

set_target_properties(kface PROPERTIES VERSION ${KFACE_LIB_SO_VERSION_STRING}
                            SOVERSION ${KFACE_LIB_SO_CUR_VERSION})

install(TARGETS kface EXPORT Kface ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/database/dbconfig.xml
        DESTINATION ${DATA_INSTALL_DIR}/libkface/database )

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
if(NOT DIGIKAMSC_USE_PRIVATE_SHAREDLIBS)

    install(FILES facedetector.h
                  recognitiondatabase.h
                  identity.h
                  dataproviders.h
                  ${CMAKE_CURRENT_BINARY_DIR}/kface_export.h
                  ${CMAKE_CURRENT_BINARY_DIR}/version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
           )

endif()
