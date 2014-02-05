# Find Detours includes and library
#
# This module defines
#  DETOURS_INCLUDE_DIR
#  DETOURS_LIBRARIES, the library to link against to use detours.
#  DETOURS_LIBARY_DIR
#  DETOURS_FOUND, If false, do not try to use detours


IF (WIN32) #Windows
    MESSAGE(STATUS "Looking for Detours in ${PROJECT_SOURCE_DIR}/detours/ first then globally")
	FIND_PATH(DETOURS_INCLUDE_DIR NAMES detours.h ${PROJECT_SOURCE_DIR}/detours/src NO_DEFAULT_PATH)
    FIND_PATH(DETOURS_INCLUDE_DIR NAMES detours.h)
	FIND_PATH(DETOURS_LIBRARY_DIR NAMES detours.lib ${PROJECT_SOURCE_DIR}/detours/lib.X86 NO_DEFAULT_PATH)
    FIND_PATH(DETOURS_LIBRARY_DIR NAMES detours.lib)
ENDIF (WIN32)


IF (DETOURS_INCLUDE_DIR AND DETOURS_LIBRARY_DIR)
    SET(DETOURS_FOUND TRUE)
    SET(DETOURS_LIBRARIES ${DETOURS_LIBRARY_DIR}/detours.lib)
ENDIF ()

IF (DETOURS_FOUND)
    MESSAGE(STATUS "  libraries : ${DETOURS_LIBRARIES}")
    MESSAGE(STATUS "  includes  : ${DETOURS_INCLUDE_DIR}")
ELSE ()
    IF (DETOURS_FIND_REQUIRED)
        MESSAGE("Include dir : ${DETOURS_INCLUDE_DIR}")
        MESSAGE("Lib dir     : ${DETOURS_LIBRARY_DIR}")
        MESSAGE(FATAL_ERROR "Could not find DETOURS")
    ENDIF ()
ENDIF ()