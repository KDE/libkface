include_directories(main
                    ${CMAKE_CURRENT_SOURCE_DIR}/../libopentld/tld
                    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/cvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libkface
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../tldinterface
                    ${OpenCV_INCLUDE_DIRS}
                    )

add_library(main    main/Main.cpp
                    main/Main.h
                    )

install(TARGETS main ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES    main/Main.h
                 DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
                 )
