# - Try to find the LibFace library
#
#  LIBFACE_MIN_VERSION - You can set this variable to the minimum version you need 
#                        before doing FIND_PACKAGE(LibFace). The default is 0.1.0.
# 
# Once done this will define
#
#  LIBFACE_FOUND - system has libface
#  LIBFACE_INCLUDE_DIR - the libface include directory
#  LIBFACE_LIBRARIES - Link these to use libface
#  LIBFACE_DEFINITIONS - Compiler switches required for using libface
#

# Copyright (c) 2010, Gilles Caulier, <caulier.gilles@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

IF (LIBFACE_INCLUDE_DIR AND LIBFACE_LIBRARIES)

  # in cache already
  MESSAGE(STATUS "Found LibFace library in cache: ${LIBFACE_LIBRARIES}")
  SET(LIBFACE_FOUND TRUE)

ELSE (LIBFACE_INCLUDE_DIR AND LIBFACE_LIBRARIES)

  IF (NOT WIN32)

    MESSAGE(STATUS "Check LibFace library using pkg-config...")

    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    INCLUDE(UsePkgConfig)

    IF(NOT LIBFACE_MIN_VERSION)
      SET(LIBFACE_MIN_VERSION "0.1")
    ENDIF(NOT LIBFACE_MIN_VERSION)

    pkgconfig(libface _LIBFACEIncDir _LIBFACELinkDir _LIBFACELinkFlags _LIBFACECflags)

    IF(_LIBFACELinkFlags)
      # query pkg-config asking for a LibFace >= 0.1
      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=${LIBFACE_MIN_VERSION} libface RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
      IF(_return_VALUE STREQUAL "0")
        MESSAGE(STATUS "Found LibFace release >= ${LIBFACE_MIN_VERSION}")
        SET(LIBFACE_VERSION_GOOD_FOUND TRUE)
      ELSE(_return_VALUE STREQUAL "0")
        MESSAGE(STATUS "Found LibFace release < ${LIBFACE_MIN_VERSION}")
      ENDIF(_return_VALUE STREQUAL "0")
    ELSE(_LIBFACELinkFlags)
        SET(LIBFACE_FOUND FALSE)
        SET(LIBFACE_VERSION_GOOD_FOUND FALSE)
        MESSAGE(STATUS "Cannot find LibFace library!")
    ENDIF(_LIBFACELinkFlags)

  ELSE(NOT WIN32)

     # Better check
     SET(LIBFACE_VERSION_GOOD_FOUND TRUE)

  ENDIF (NOT WIN32)

  IF(LIBFACE_VERSION_GOOD_FOUND)

     SET(LIBFACE_DEFINITIONS ${_LIBFACECflags})

     FIND_PATH(LIBFACE_INCLUDE_DIR NAMES libface/Face.h
       HINTS
       ${_LIBFACEIncDir}
     )

     FIND_LIBRARY(LIBFACE_LIBRARIES NAMES face libface
       HINTS
       ${_LIBFACELinkDir}
     )

     IF (LIBFACE_INCLUDE_DIR AND LIBFACE_LIBRARIES)

        SET(LIBFACE_FOUND TRUE)
        # TODO version check is missing

     ENDIF (LIBFACE_INCLUDE_DIR AND LIBFACE_LIBRARIES)

   ENDIF(LIBFACE_VERSION_GOOD_FOUND)

   IF (LIBFACE_FOUND)

      IF (NOT Face_FIND_QUIETLY)
       MESSAGE(STATUS "Found Face: ${LIBFACE_LIBRARIES}")
      ENDIF (NOT Face_FIND_QUIETLY)

   ELSE (LIBFACE_FOUND)

     IF (Face_FIND_REQUIRED)
       IF (NOT LIBFACE_INCLUDE_DIR)
         MESSAGE(FATAL_ERROR "Could NOT find LibFace header files")
       ENDIF (NOT LIBFACE_INCLUDE_DIR)
       IF (NOT LIBFACE_LIBRARIES)
           MESSAGE(FATAL_ERROR "Could NOT find LibFace library")
       ENDIF (NOT LIBFACE_LIBRARIES)
     ENDIF (Face_FIND_REQUIRED)

   ENDIF (LIBFACE_FOUND)

  MARK_AS_ADVANCED(LIBFACE_INCLUDE_DIR LIBFACE_LIBRARIES)

ENDIF (LIBFACE_INCLUDE_DIR AND LIBFACE_LIBRARIES)
