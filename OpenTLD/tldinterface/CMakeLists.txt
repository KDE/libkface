include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libkface
                    ${CMAKE_CURRENT_SOURCE_DIR}/../src/opentld/main
                    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libopentld/tld
                    )

add_library(tldinterface    tldclassify.cpp
                            tlddatabase.cpp
                            tldface.cpp
                            tldrecognition.cpp
                            tldclassify.h
                            tlddatabase.h
                            tldface.h
                            tldrecognition.h
                            )

install(TARGETS tldinterface ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(tldinterface main opentld cvblobs sqlite3 QtSql  ${OpenCV_LIBS})

install(FILES    tldclassify.h
                 tlddatabase.h
                 tldface.h
                 tldrecognition.h
                 DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
                 )
