include_directories(../../libkface
                    ../src/opentld/main
                    ../src/libopentld/tld
                    /usr/lib/qt4
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
target_link_libraries(tldinterface libkface sqlite3 QtSql)