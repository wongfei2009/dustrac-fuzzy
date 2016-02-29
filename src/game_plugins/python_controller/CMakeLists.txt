# Sources.
set(PythonControllerSRC
	pydata.cpp
	pythoncontroller.cpp
	pylistener.cpp
	pythonexception.cpp
	loader.cpp
)

add_plugin(PythonController ${PythonControllerSRC})

if(BUILD_PythonController)
	FIND_PACKAGE(PythonLibs REQUIRED)
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

	target_link_libraries(PythonController ${PYTHON_LIBRARIES})

	# copy/install the python header with the bindings
	FILE(COPY bindings.py DESTINATION ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}/PythonController)
	install(FILES bindings.py DESTINATION "${PLUGIN_INSTALL_PATH}/PythonController")

	# copy/install a default controller file
	FILE(COPY controller.py DESTINATION ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}/PythonController)
	install(FILES controller.py DESTINATION "${PLUGIN_INSTALL_PATH}/PythonController")

	install(TARGETS PythonController DESTINATION "${PLUGIN_INSTALL_PATH}/PythonController")
endif()
