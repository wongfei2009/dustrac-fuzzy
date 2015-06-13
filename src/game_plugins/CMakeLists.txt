set(GAME_DIR "${CMAKE_SOURCE_DIR}/src/game")

# The base include path.
include_directories(${GAME_DIR})

# Engine include paths
include_directories("${GAME_DIR}/MiniCore/Asset")
include_directories("${GAME_DIR}/MiniCore/Core")
include_directories("${GAME_DIR}/MiniCore/Graphics")
include_directories("${GAME_DIR}/MiniCore/Particles")
include_directories("${GAME_DIR}/MiniCore/Physics")
include_directories("${GAME_DIR}/MiniCore/Text")

# Menu toolkit include paths
include_directories("${GAME_DIR}/MTFH")

# Sound toolkit include paths
include_directories("${GAME_DIR}/STFH")

macro(add_plugin NAME)
	option("BUILD_${NAME}" "Enable support for plugin ${NAME}." ON)

	if("BUILD_${NAME}")
		add_library(${NAME} SHARED ${ARGN})
		set_target_properties(FuzzyController
			 PROPERTIES
			 PREFIX ""
			 SUFFIX ".dpl"
			 LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
		)
		# Some Qt deps.
		qt5_use_modules(${NAME} Widgets Xml)

		MESSAGE("FINISHED ADDING THE STUFF")
	endif()
endmacro(add_plugin)

# The individual plugins:
add_subdirectory(fuzzy_controller)

