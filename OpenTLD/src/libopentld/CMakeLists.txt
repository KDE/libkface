include_directories(tld
                    ../3rdparty/cvblobs
                    ../../tldinterface
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

