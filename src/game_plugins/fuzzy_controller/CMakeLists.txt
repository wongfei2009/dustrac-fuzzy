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
	find_package(fuzzylite REQUIRED)
	include_directories("${fuzzylite_INCLUDE_DIR}")

	target_link_libraries(FuzzyController dustrac_lib ${fuzzylite_LIBRARIES})

	# copy/install a default controller file
	FILE(COPY controller.fis DESTINATION ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}/FuzzyController)
	install(FILES controller.fis DESTINATION "${PLUGIN_INSTALL_PATH}/FuzzyController")
                                                                                
	install(TARGETS FuzzyController DESTINATION "${PLUGIN_INSTALL_PATH}/FuzzyController")
endif()
