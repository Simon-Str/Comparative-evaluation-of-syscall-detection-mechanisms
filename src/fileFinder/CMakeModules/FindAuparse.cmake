# - Find Libaudit
# This module looks for Crypto++ library support
# it will define the following values
#  AUPARSE_INCLUDE_DIR  = directory containing AUPARSE
#  AUPARSE_LINK_DIR	 = directory containing AUPARSE
#  AUPARSE_LIBRARY   	 = AUPARSE library
#  AUPARSE_FOUND        = true -> all files were found

# Assume that everything is ok
SET(AUPARSE_FOUND "YES")

#
# AUPARSE.h
#
FIND_PATH(AUPARSE_INCLUDE_DIR
        auparse.h
        PATHS /usr/local/include/* /usr/include/*
        )

IF (NOT AUPARSE_INCLUDE_DIR)
    SET(AUPARSE_FOUND "NO")
    IF (AUPARSE_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR  "Could not find auparse.h")
    ENDIF (AUPARSE_FIND_REQUIRED)
ENDIF (NOT AUPARSE_INCLUDE_DIR)

#
#  library
#
FIND_LIBRARY(AUPARSE_LIBRARY
        NAMES libauparse.so
        PATHS /usr/local/lib /usr/lib
        )

IF (NOT AUPARSE_LIBRARY)
    SET(AUPARSE_FOUND "NO")
    IF (AUPARSE_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR  "Could not find AUPARSE library")
    ENDIF (AUPARSE_FIND_REQUIRED)
ENDIF (NOT AUPARSE_LIBRARY)

GET_FILENAME_COMPONENT(AUPARSE_LINK_DIR ${AUPARSE_LIBRARY} PATH )

# Checking ...
IF (AUPARSE_FOUND)
    IF (NOT AUPARSE_FIND_QUIETLY)
        MESSAGE(STATUS "Found auparse.h in..: ${AUPARSE_INCLUDE_DIR}")
        MESSAGE(STATUS "Found AUPARSE lib...: ${AUPARSE_LIBRARY}")
        MESSAGE(STATUS "Library link dir.....: ${AUPARSE_LINK_DIR}")
    ENDIF (NOT AUPARSE_FIND_QUIETLY)
ELSE (AUPARSE_FOUND)
    IF (NOT AUPARSE_FIND_QUIETLY)
        MESSAGE(FATAL_ERROR  "Could not find AUPARSE library")
    ENDIF (NOT AUPARSE_FIND_QUIETLY)
ENDIF (AUPARSE_FOUND)


MARK_AS_ADVANCED(
        AUPARSE_INCLUDE_DIR
        AUPARSE_LIBRARY
)
