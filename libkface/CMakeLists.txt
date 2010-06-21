
INCLUDE_DIRECTORIES( ${LIBFACE_INCLUDE_DIR}
                     ${OpenCV_CV_INCLUDE_DIR}
                     ${OpenCV_CXCORE_INCLUDE_DIR}
                     ${OpenCV_CVAUX_INCLUDE_DIR}
                     ${OpenCV_HIGHGUI_INCLUDE_DIR}
                   )

ADD_DEFINITIONS(${LIBFACE_DEFINITIONS})
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})
REMOVE_DEFINITIONS(-DQT_NO_STL)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(kface_LIB_SRCS database.cpp
                   kface.cpp
                   kfaceutils.cpp
                   image.cpp
   )

KDE4_ADD_LIBRARY(kface SHARED ${kface_LIB_SRCS})

TARGET_LINK_LIBRARIES(kface ${KDE4_KDECORE_LIBS}
                            ${QT_QTGUI_LIBRARY}

#                            ${LIBFACE_LIBRARIES}
                            -lface

                            ${OpenCV_HIGHGUI_LIBRARY}
                            ${OpenCV_CV_LIBRARY}
                            ${OpenCV_CXCORE_LIBRARY}
                            ${OpenCV_CVAUX_LIBRARY}
                            )

SET_TARGET_PROPERTIES(kface PROPERTIES VERSION ${KFACE_LIB_SO_VERSION_STRING}
                            SOVERSION ${KFACE_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kface ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES database.h
              kface.h
              kfaceutils.h
              libkface_export.h
              image.h
              ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
       )
