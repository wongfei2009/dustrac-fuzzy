# Sources.
set(PythonControllerSRC
	pydata.cpp
	pythoncontroller.cpp
	pylistener.cpp
	loader.cpp
)

add_plugin(PythonController ${PythonControllerSRC})

if(BUILD_PythonController)
	FIND_PACKAGE(PythonLibs REQUIRED)
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

	target_link_libraries(PythonController ${PYTHON_LIBRARIES})

	# copy/install a default controller file
	FILE(COPY controller.py DESTINATION ${CMAKE_BINARY_DIR}/data)
	install(FILES controller.py DESTINATION data)
endif()
