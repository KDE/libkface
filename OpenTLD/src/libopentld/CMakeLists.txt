include_directories(tld
                    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/cvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libkface
                    )

link_directories(${OpenCV_LIB_DIR})

add_library(opentld    tld/Clustering.cpp
                       tld/DetectionResult.cpp
                       tld/DetectorCascade.cpp
                       tld/EnsembleClassifier.cpp
                       tld/ForegroundDetector.cpp
                       tld/NNClassifier.cpp
                       tld/TLD.cpp
                       tld/TLDUtil.cpp
                       tld/VarianceFilter.cpp
                       tld/Clustering.h
                       tld/DetectionResult.h
                       tld/DetectorCascade.h
                       tld/EnsembleClassifier.h
                       tld/ForegroundDetector.h
                       tld/NNClassifier.h
                       tld/TLD.h
                       tld/TLDUtil.h
                       tld/VarianceFilter.h
                       )

target_link_libraries(opentld cvblobs)

#install(TARGETS opentld ${INSTALL_TARGETS_DEFAULT_ARGS})

#install(FILES    tld/Clustering.h
#                 tld/DetectionResult.h
#                 tld/DetectorCascade.h
#                 tld/EnsembleClassifier.h
#                 tld/ForegroundDetector.h
#                 tld/NNClassifier.h
#                 tld/TLD.h
#                 tld/TLDUtil.h
#                 tld/VarianceFilter.h
#                 tld/IntegralImage.h
#                 tld/NormalizedPatch.h
#        DESTINATION ${INCLUDE_INSTALL_DIR}/libkface COMPONENT Devel
#       )
