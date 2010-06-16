
INCLUDE_DIRECTORIES(${LIBFACE_INCLUDE_DIR})

ADD_DEFINITIONS(${LIBFACE_DEFINITIONS})
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})
REMOVE_DEFINITIONS(-DQT_NO_STL)

FIND_PACKAGE(OpenCV REQUIRED)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# This was used to enable catching of exceptions thrown by libexiv2.
# We use now #pragma GCC visibility push(default) around the headers
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")

SET(kface_LIB_SRCS database.cpp
                   kface.cpp
                   kfaceutils.cpp
   )

KDE4_ADD_LIBRARY(kface SHARED ${kface_LIB_SRCS})

TARGET_LINK_LIBRARIES(kface ${KDE4_KDECORE_LIBS}
                            ${QT_QTGUI_LIBRARY}
                            -lface -lhighgui -lcv -lcxcore -lcvaux
#                            ${LIBFACE_LIBRARIES}
                     )

SET_TARGET_PROPERTIES(kface PROPERTIES VERSION ${KFACE_LIB_SO_VERSION_STRING}
                            SOVERSION ${KFACE_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kface ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES database.h
              kface.h
              kfaceutils.hh
              libkface_export.h
              version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel)
