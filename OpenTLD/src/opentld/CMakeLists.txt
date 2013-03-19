include_directories(main
                    ${CMAKE_CURRENT_SOURCE_DIR}/../libopentld/tld
                    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/cvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libkface
                    ${OpenCV_INCLUDE_DIRS}
                    )

add_library(main    main/Main.cpp
                    )

#install(TARGETS main ${INSTALL_TARGETS_DEFAULT_ARGS})

#install(FILES    main/Main.h
#        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
#       )
