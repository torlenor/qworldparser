# CmakeLists.txt for QWorldParser-gui

# Used source files are specified here
set(CPP_SOURCE_FILES
    main.cpp
    heightdata.cpp
    heightmapscatterplot.cpp
    osmparser.cpp
    qworldparser.cpp qworldparser.ui
    srtmparser.cpp
    # qworldparser_resources.qrc
    # qworldparser_icon.rc
)

set (CMAKE_PREFIX_PATH ${QT_ROOT_PATH})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# QT run MOC and UIC automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DSETTINGS_COMPANY="${COMPANY}" -DSETTINGS_PRODUCT="${PROJECT_NAME}" -DCURRARCH="x86" -DVERSION="${QWORLDPARSER_VERSION}")

# Find the Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5DataVisualization REQUIRED)
find_package(Qt5Charts REQUIRED)

set(LINK_TO_LIBS Qt5::Widgets Qt5::OpenGL Qt5::DataVisualization Qt5::Charts)

# For Apple set the icns file containing icons
IF(APPLE)
    SET(MACOSX_BUNDLE_ICON_FILE QWorldParser.icns)
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/QWorldParser.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    LIST(APPEND CPP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/QWorldParser.icns)
ENDIF(APPLE)

if(WIN32)
    add_executable(QWorldParser WIN32 ${CPP_SOURCE_FILES})
elseif(APPLE)
    add_executable(QWorldParser MACOSX_BUNDLE ${CPP_SOURCE_FILES})
else()
    add_executable(QWorldParser ${CPP_SOURCE_FILES})
endif()
target_link_libraries(QWorldParser ${LINK_TO_LIBS})

# ------------------------------------------------------------------------- #
# INSTALL SECTION
# ------------------------------------------------------------------------- #
if(APPLE)
    install(TARGETS QWorldParser
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION bin COMPONENT Runtime)
else()
    install(TARGETS QWorldParser
        RUNTIME DESTINATION bin)
endif()
   
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtGui_location Qt5::Gui LOCATION)
get_target_property(QtOpenGL_location Qt5::OpenGL LOCATION)
get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
get_target_property(QtCharts_location Qt5::Charts LOCATION)
get_target_property(QtDataVisualization_location Qt5::DataVisualization LOCATION)

if(MINGW AND WIN32)
    find_library(libwinpthread-1_location libwinpthread-1)
    message(STATUS "Installing ${libwinpthread-1_location}")
    install(FILES 	${libwinpthread-1_location}
            DESTINATION bin)

    find_library(libgcc_s_dw2-1_location libgcc_s_dw2-1)
    message(STATUS "Installing ${libgcc_s_dw2-1_location}")
    install(FILES 	${libgcc_s_dw2-1_location}
            DESTINATION bin)

    find_library(libstdc++-6_location libstdc++-6)
    message(STATUS "Installing ${libstdc++-6_location}")
    install(FILES 	${libstdc++-6_location}
            DESTINATION bin )
            # BUNDLE_DESTINATION bin)
endif(MINGW AND WIN32)

if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES_OLD ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .dll .dll.a .a)
    find_library(ssleay32_location ssleay32
                 PATHS ${SSL_DIR} NO_DEFAULT_PATH)
    find_library(libeay32_location libeay32
                 PATHS ${SSL_DIR} NO_DEFAULT_PATH)
    message(STATUS "Installing ${ssleay32_location}")
    message(STATUS "Installing ${libeay32_location}")
    install(FILES   ${ssleay32_location}
                    ${libeay32_location}
            DESTINATION bin)
  
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_OLD})
endif()

if(WIN32 OR APPLE)
    message(STATUS "Installing ${QtGui_location}")
    message(STATUS "Installing ${QtCore_location}")
    message(STATUS "Installing ${QtOpenGL_location}")
    message(STATUS "Installing ${QtWidgets_location}")
    message(STATUS "Installing ${QtDataVisualization_location}")
    message(STATUS "Installing ${QtCharts_location}")

    install(FILES   ${QtCore_location} 
                    ${QtGui_location}
                    ${QtOpenGL_location}
                    ${QtWidgets_location}
                    ${QtDataVisualization_location}
                    ${QtCharts_location}
            DESTINATION bin)

    foreach(plugin ${Qt5Gui_PLUGINS})
        get_target_property(_loc ${plugin} LOCATION)
        message(STATUS "Installing plugin for ${plugin}: ${_loc}")
        get_filename_component(_loc_path "${_loc}" PATH)
        get_filename_component(_loc_parent_path "${_loc_path}" PATH)
        get_filename_component(_loc_dir_name "${_loc_path}" NAME)
        get_filename_component(_loc_name "${_loc}" NAME)
        install(FILES ${_loc}
                DESTINATION bin/${_loc_dir_name})
    endforeach()

endif(WIN32 OR APPLE)

set(CPACK_PACKAGE_NAME "QWorldParser")
set(CPACK_PACKAGE_VERSION ${QWORLDPARSER_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${QWORLDPARSER_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${QWORLDPARSER_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${QWORLDPARSER_PATCH_VERSION})
set(QWORLDPARSER_REVISION_VERSION ${QWORLDPARSER_PATCH_VERSION})

set(CPACK_PACKAGE_CONTACT "hps")

SET(CPACK_NSIS_EXECUTABLES_DIRECTORY ".") 
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QWorldParser")
SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/QWorldParser.ico")
SET(CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "  CreateShortCut '$DESKTOP\\\\QWorldParser.lnk' '$INSTDIR\\\\bin\\\\QWorldParser.exe' ")    
	
if(CPACK_GENERATOR STREQUAL "DEB")
    string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
    find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
    if(DPKG_PROGRAM)
      execute_process(
        COMMAND ${DPKG_PROGRAM} --print-architecture
        OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      set(CPACK_PACKAGE_FILE_NAME
          "${CPACK_PACKAGE_NAME_LOWERCASE}_${QWORLDPARSER_VERSION}-${QWORLDPARSER_REVISION_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")   
    else(DPKG_PROGRAM)
            set(CPACK_PACKAGE_FILE_NAME
          "${CPACK_PACKAGE_NAME_LOWERCASE}_${QWORLDPARSER_VERSION}-${QWORLDPARSER_REVISION_VERSION}_${CMAKE_SYSTEM_NAME}")
    endif(DPKG_PROGRAM)

endif()

# This must always be last!
include(CPack)
