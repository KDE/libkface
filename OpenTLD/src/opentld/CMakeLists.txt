include_directories(main
                    ../libopentld/tld
                    ../3rdparty/cvblobs
                    ../../../libkface
                    ../../tldinterface
                    ${OpenCV_INCLUDE_DIRS}
                    )

add_library(main    main/Main.cpp
                    main/Main.h
                    )
target_link_libraries(main opentld cvblobs sqlite3 QtSql ${OpenCV_LIBS})
