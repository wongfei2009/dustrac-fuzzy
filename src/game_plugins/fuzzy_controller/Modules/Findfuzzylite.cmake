# - Try to find fuzzylite
# This module reads hints about search locations from variables:
# fuzzylite_DIR Hint about the location of the entire project.
# fuzzylite_INCLUDEDIR Hint about the location of the includes.
# fuzzylite_LIBRARYDIR Hint about the location of the libraries.
#
# Once done this will define
#  fuzzylite_FOUND - System has fuzzylite
#  fuzzylite_INCLUDE_DIR - The fuzzylite include directories
#  fuzzylite_LIBRARIES - The libraries needed to use fuzzylite
#  fuzzylite_DEFINITIONS - Compiler switches required for using fuzzylite

macro(_FIND_FUZZYLITE_LIBRARY _var)
	find_library(${_var}
		NAMES
			${ARGN}
		HINTS 
			${PC_fuzzylite_LIBDIR}
			${PC_fuzzylite_LIBRARY_DIRS}
	)
	mark_as_advanced(${_var})
endmacro()

find_package(PkgConfig)
pkg_check_modules(PC_fuzzylite QUIET fuzzylite)
set(fuzzylite_DEFINITIONS ${PC_fuzzylite_CFLAGS_OTHER})

find_path(fuzzylite_INCLUDE_DIR fuzzylite.h
          HINTS ${PC_fuzzylite_INCLUDEDIR} ${PC_fuzzylite_INCLUDE_DIRS}
          PATH_SUFFIXES fl )

_FIND_FUZZYLITE_LIBRARY(fuzzylite_Library	fuzzylite)

set(fuzzylite_LIBRARIES
		${fuzzylite_Library}
)

set(fuzzylite_INCLUDE_DIRS ${fuzzylite_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set fuzzylite_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(fuzzylite  DEFAULT_MSG
	fuzzylite_Library
	fuzzylite_INCLUDE_DIR)

mark_as_advanced(fuzzylite_INCLUDE_DIR fuzzylite_LIBRARY )
