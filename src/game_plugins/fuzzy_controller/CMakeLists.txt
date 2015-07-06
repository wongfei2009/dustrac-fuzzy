# Add custom search modules.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/Modules/")

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

	# copy/install a default controller file
	FILE(COPY controller.fis DESTINATION ${CMAKE_BINARY_DIR}/data)
	install(FILES controller.fis DESTINATION data)
endif()
