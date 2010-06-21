INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkface)

SET(detect_SRCS detect.cpp)
KDE4_ADD_EXECUTABLE(detect ${detect_SRCS})
TARGET_LINK_LIBRARIES(detect kface ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

# -----------------------------------------------------------------------------


SET(libfaceGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/gui/faceitem.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.cpp
   )

KDE4_ADD_UI_FILES(libfaceGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui)

KDE4_ADD_EXECUTABLE(libfaceGUI ${libfaceGUI_SRCS})
TARGET_LINK_LIBRARIES(libfaceGUI kface ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
