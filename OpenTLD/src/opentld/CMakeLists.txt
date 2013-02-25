include_directories(main
                    ../libopentld/mftracker
                    ../libopentld/tld
                    ../3rdparty/cvblobs
                    ../../../libkface
                    ../../tldinterface
                    ${OpenCV_INCLUDE_DIRS}
                    )

add_library(main    main/Config.cpp
                    main/Main.cpp
                    main/Settings.cpp
                    main/Config.h
                    main/Main.h
                    main/Settings.h)

target_link_libraries(main opentld cvblobs sqlite3 QtSql ${OpenCV_LIBS})
