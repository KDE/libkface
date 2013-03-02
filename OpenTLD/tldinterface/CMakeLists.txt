include_directories(../../libkface
                    ../src/opentld/main
                    ../src/libopentld/tld
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
target_link_libraries(tldinterface main)
