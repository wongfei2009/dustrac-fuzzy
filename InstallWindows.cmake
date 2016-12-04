# **** Resolve install and data paths ****
function(resolve_install_paths)

    message(STATUS "Windows build.")
    
    set(BIN_PATH .)
    set(DATA_PATH ./data)
	set(PLUGIN_INSTALL_PATH ./${PLUGIN_PATH})
    set(DOC_PATH .)

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

    # Add target to copy runtime files to the binary dir.
    add_custom_target(runtimeData ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README
        DEPENDS ${GAME_BINARY_NAME} ${QM_FILES})

    setup_install_targets(${BIN_PATH} ${DATA_PATH} ${PLUGIN_INSTALL_PATH} ${DOC_PATH})

	# export the path to plugins into the parent scope
	set(PLUGIN_INSTALL_PATH ${PLUGIN_INSTALL_PATH} PARENT_SCOPE)

endfunction()

# **** Instal targets for Windows ****
function(setup_install_targets BIN_PATH DATA_PATH PLUGIN_INSTALL_PATH DOC_PATH)

    # Install binaries and game data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${GAME_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${EDITOR_BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/surfaces.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")
    install(DIRECTORY data/translations DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.qm")

endfunction()

