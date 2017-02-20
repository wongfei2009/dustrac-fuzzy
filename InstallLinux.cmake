# **** Resolve install and data paths ****
function(resolve_install_paths)

    # Set default install paths for release builds.
    set(DEFAULT_DATA_PATH_BASE share/games/DustRacing2D)
    if(NOT DATA_PATH)
        set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/data)
    endif()
    if(NOT PLUGIN_INSTALL_PATH)
        set(PLUGIN_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/${PLUGIN_PATH})
    endif()
    if(NOT BIN_PATH)
        set(BIN_PATH bin)
    endif()
    if(NOT LIB_PATH)
        set(LIB_PATH lib)
    endif()
    if(NOT DOC_PATH)
        set(DOC_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/)
    endif()

    if(ReleaseBuild)
        message(STATUS "Linux release build with system install targets.")
        if(USC)
            message(STATUS "Installing to /opt.")
            set(BIN_PATH /opt/dustrac)
            set(LIB_PATH /opt/dustrac)
            set(DATA_PATH /opt/dustrac/data)
			set(PLUGIN_INSTALL_PATH /opt/dustrac/${PLUGIN_PATH})
            set(DOC_PATH /opt/dustrac/)
        endif()
    else()
        message(STATUS "Linux development build with local install targets.")
        set(BIN_PATH .)
        set(LIB_PATH .)
        set(DATA_PATH ./data)
        set(PLUGIN_INSTALL_PATH ./${PLUGIN_PATH})
        set(DOC_PATH .)

        # Add target to copy runtime files to the binary dir.
        add_custom_target(runtime ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README
            DEPENDS ${GAME_BINARY_NAME} ${QM_FILES})
    endif()

    message(STATUS "BIN_PATH: " ${BIN_PATH})
    message(STATUS "LIB_PATH: " ${LIB_PATH})
    message(STATUS "DATA_PATH: " ${DATA_PATH})
    message(STATUS "PLUGIN_INSTALL_PATH: " ${PLUGIN_INSTALL_PATH})
    message(STATUS "DOC_PATH:" ${DOC_PATH})

    # This is the main data path given to the game and editor binaries.
    add_definitions(-DDATA_PATH="${DATA_PATH}")

    setup_install_targets(${BIN_PATH} ${LIB_PATH} ${DATA_PATH} ${PLUGIN_INSTALL_PATH} ${DOC_PATH})

	# export the path to plugins into the parent scope
	set(PLUGIN_INSTALL_PATH ${PLUGIN_INSTALL_PATH} PARENT_SCOPE)
	set(BIN_PATH ${BIN_PATH} PARENT_SCOPE)
	set(LIB_PATH ${LIB_PATH} PARENT_SCOPE)

endfunction()

# **** Install targets for Linux ****
function(setup_install_targets BIN_PATH LIB_PATH DATA_PATH PLUGIN_INSTALL_PATH DOC_PATH)

    # Install game data
    install(FILES data/editorModels.conf DESTINATION ${DATA_PATH})
    install(FILES data/fonts.conf DESTINATION ${DATA_PATH})
    install(FILES data/meshes.conf DESTINATION ${DATA_PATH})
    install(FILES data/surfaces.conf DESTINATION ${DATA_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README DESTINATION ${DOC_PATH})
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.jpg")
    install(DIRECTORY data/images DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.png")
    install(DIRECTORY data/levels DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.trk")
    install(DIRECTORY data/models DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.obj")
    install(DIRECTORY data/sounds DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ogg")
    install(DIRECTORY data/fonts DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.ttf")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/data/translations DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.qm")

    if(ReleaseBuild)
        # Install .desktop files
        install(FILES ${CMAKE_BINARY_DIR}/dustrac-game.desktop DESTINATION share/applications)
        install(FILES ${CMAKE_BINARY_DIR}/dustrac-editor.desktop DESTINATION share/applications)

        # Install app store meta data
        install(FILES src/dustrac.appdata.xml DESTINATION share/appdata)

        # Install icons
        install(FILES data/icons/dustrac-game.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-game.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/pixmaps)
        install(FILES data/icons/dustrac-editor.png DESTINATION share/icons/hicolor/64x64/apps)

        # Create symlinks to targets
        if(USC)
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${GAME_BINARY_NAME} /usr/bin/${GAME_BINARY_NAME})
            execute_process(COMMAND cmake -E create_symlink ${BIN_PATH}/${EDITOR_BINARY_NAME} /usr/bin/${EDITOR_BINARY_NAME})
        endif()
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ ON)
    set(CPACK_BINARY_TGZ ON)
    set(CPACK_BINARY_TZ OFF)

    set(QT_VER_STR "qt5")

    set(CPACK_PACKAGE_FILE_NAME "dustrac-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

    if(NOT ReleaseBuild)
        include(CPack)
    endif()
    
endfunction()
