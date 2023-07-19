# - Find Libaudit
# This module looks for Crypto++ library support
# it will define the following values
#  LIBAUDIT_INCLUDE_DIR  = directory containing LIBAUDIT
#  LIBAUDIT_LINK_DIR	 = directory containing LIBAUDIT
#  LIBAUDIT_LIBRARY   	 = LIBAUDIT library
#  LIBAUDIT_FOUND        = true -> all files were found

# Assume that everything is ok
SET(LIBAUDIT_FOUND "YES")

#
# libaudit.h
#
FIND_PATH(LIBAUDIT_INCLUDE_DIR
        libaudit.h
        PATHS /usr/local/include/* /usr/include/*
        )

IF (NOT LIBAUDIT_INCLUDE_DIR)
    SET(LIBAUDIT_FOUND "NO")
    IF (LIBAUDIT_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR  "Could not find libaudit.h")
    ENDIF (LIBAUDIT_FIND_REQUIRED)
ENDIF (NOT LIBAUDIT_INCLUDE_DIR)

#
#  library
#
FIND_LIBRARY(LIBAUDIT_LIBRARY
        NAMES libaudit.so
        PATHS /usr/local/lib /usr/lib
        )

IF (NOT LIBAUDIT_LIBRARY)
    SET(LIBAUDIT_FOUND "NO")
    IF (LIBAUDIT_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR  "Could not find libaudit library")
    ENDIF (LIBAUDIT_FIND_REQUIRED)
ENDIF (NOT LIBAUDIT_LIBRARY)

GET_FILENAME_COMPONENT(LIBAUDIT_LINK_DIR ${LIBAUDIT_LIBRARY} PATH )


# Checking ...
IF (LIBAUDIT_FOUND)
    IF (NOT LIBAUDIT_FIND_QUIETLY)
        MESSAGE(STATUS "Found libaudit.h in..: ${LIBAUDIT_INCLUDE_DIR}")
        MESSAGE(STATUS "Found libaudit lib...: ${LIBAUDIT_LIBRARY}")
        MESSAGE(STATUS "Library link dir.....: ${LIBAUDIT_LINK_DIR}")
    ENDIF (NOT LIBAUDIT_FIND_QUIETLY)
ELSE (LIBAUDIT_FOUND)
    IF (NOT LIBAUDIT_FIND_QUIETLY)
        MESSAGE(FATAL_ERROR  "Could not find libaudit library")
    ENDIF (NOT LIBAUDIT_FIND_QUIETLY)
ENDIF (LIBAUDIT_FOUND)


MARK_AS_ADVANCED(
        LIBAUDIT_INCLUDE_DIR
        LIBAUDIT_LIBRARY
)
