# Sources.
set(FuzzyControllerSRC
	fuzzycontroller.cpp
	loader.cpp
)

add_plugin(FuzzyController ${FuzzyControllerSRC})

if(BUILD_FuzzyController)
	# Find fuzzylite.
	find_package(fuzzylite)
	include_directories("${fuzzylite_INCLUDE_DIRS}")

	target_link_libraries(FuzzyController ${fuzzylite_LIBRARIES})
endif()
