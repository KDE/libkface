ADD_LIBRARY(cvblobs    blob.cpp
                       BlobContour.cpp
                       BlobOperators.cpp
                       BlobProperties.cpp
                       BlobResult.cpp
                       ComponentLabeling.cpp
                       )
#install(TARGETS cvblobs ${INSTALL_TARGETS_DEFAULT_ARGS})

#INSTALL(FILES    blob.h
#                 BlobContour.h
#                 BlobOperators.h
#                 BlobProperties.h
#                 BlobResult.h
#                 ComponentLabeling.h
#                 BlobLibraryConfiguration.h
#                 DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
#                 )
