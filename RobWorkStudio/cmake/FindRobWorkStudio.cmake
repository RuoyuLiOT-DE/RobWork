# Find and sets up RobWorkStudio
# 
#  ROBWORKSTUDIO_INCLUDE_DIR - Where to find robwork include sub-directory.
#  ROBWORKSTUDIO_LIBRARIES   - List of libraries when using RobWork (includes all libraries that RobWork depends on).
#  ROBWORKSTUDIO_LIBRARY_DIRS - List of directories where libraries of RobWork are located. 
#  ROBWORKSTUDIO_FOUND       - True if RobWork was found. (not impl yet)
#
#  RWSTUDIO_ROOT             - If set this defines the root of ROBWORKSTUDIO if not set then it
#                              if possible be autodetected.

# Allow the syntax else (), endif (), etc.
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)

# Get the compiler architecture
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(AMD64 1)
ELSE()
	SET(AMD64 0)
ENDIF()

IF(DEFINED RWS_ROOT)
	FILE(TO_CMAKE_PATH ${RWS_ROOT} RWS_ROOT)
	SET(RWSTUDIO_ROOT ${RWS_ROOT})
ENDIF()
IF(DEFINED RWSTUDIO_ROOT)
	FILE(TO_CMAKE_PATH ${RWSTUDIO_ROOT} RWSTUDIO_ROOT)
	IF(NOT DEFINED RWS_ROOT)
		SET(RWS_ROOT "${RWSTUDIO_ROOT}")
	ENDIF()
ENDIF()
IF(DEFINED ROBWORKSTUDIO_ROOT)
	FILE(TO_CMAKE_PATH ${ROBWORKSTUDIO_ROOT} ROBWORKSTUDIO_ROOT)
	SET(RWSTUDIO_ROOT ${ROBWORKSTUDIO_ROOT})
	IF(NOT DEFINED RWS_ROOT)
		SET(RWS_ROOT "${ROBWORKSTUDIO_ROOT}")
	ENDIF()
ENDIF()

# Check if RW_ROOT path are setup correctly
FIND_FILE(ROBWORKSTUDIO_FOUND RobWorkStudioSetup.cmake 
    "${RWSTUDIO_ROOT}/cmake/" 
  	"${ROBWORKSTUDIO_ROOT}/cmake/"
	"${RWS_ROOT}/cmake/"
	"../cmake/"
	"../RobWorkStudio/cmake/"
	"../../RobWorkStudio/cmake/"
	"c:/program files/RobWork/cmake/"
	"c:/programmer/RobWork/cmake/"
)

IF(NOT ROBWORKSTUDIO_FOUND)
 MESSAGE(SEND_ERROR "Path to RobWorkStudio root (RWSTUDIO_ROOT) is incorrectly setup! \nRWSTUDIO_ROOT == ${RWSTUDIO_ROOT}")
ENDIF()
#MESSAGE(STATUS "RobWorkStudio root: ${RWSTUDIO_ROOT}")

# Add extra linker flags for linking with OpenGL on mac os computers
if (DEFINED APPLE)
  set(flags
    "-dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flags}")
endif ()

# get the build configuration of the requested built type
IF(EXISTS ${RWS_ROOT}/cmake/RobWorkStudioBuildConfig_${CMAKE_BUILD_TYPE}.cmake)
	INCLUDE(${RWS_ROOT}/cmake/RobWorkStudioBuildConfig_${CMAKE_BUILD_TYPE}.cmake)
ELSE()
	#INCLUDE(${RWS_ROOT}/build/RobWorkStudioBuildConfig.cmake)
	MESSAGE(FATAL_ERROR "Could not find ${RWS_ROOT}/cmake/RobWorkStudioBuildConfig${CMAKE_BUILD_TYPE}.cmake")
ENDIF()

#MESSAGE(STATUS "RobWorkStudio VERSION: ${RWS_BUILD_WITH_VERSION}")

IF(NOT DEFINED QTDIR)
#	SET(QTDIR "${RWS_BUILD_WITH_QTDIR}")
ENDIF()
IF(NOT DEFINED QT_HEADERS_DIR)
#	SET(QT_HEADERS_DIR "${RWS_BUILD_WITH_QT_HEADERS_DIR}")
ENDIF()
IF(NOT DEFINED QT_INCLUDE_DIR)
#	SET(QT_INCLUDE_DIR "${RWS_BUILD_WITH_QT_INCLUDE_DIR}")
ENDIF()
IF(NOT DEFINED QT_LIBRARY_DIR)
#	SET(QT_LIBRARY_DIR "${RWS_BUILD_WITH_QT_LIBRARY_DIR}")
ENDIF()
IF(NOT DEFINED QT_BINARY_DIR)
#	SET(QT_BINARY_DIR "${RWS_BUILD_WITH_QT_BINARY_DIR}")
ENDIF()

# Find and setup Qt.
SET(RWS_USE_QT5 ${RWS_BUILD_WITH_QT5})
IF(NOT RWS_USE_QT5)
	FIND_PACKAGE(Qt4 REQUIRED)
	SET(QT_USE_QTOPENGL 1)
	SET(QT_USE_QTDESIGNER 1)
	SET(QT_USE_QTUITOOLS 1)
	INCLUDE(${QT_USE_FILE})
ELSE()
	cmake_minimum_required(VERSION 2.8.3)
	FIND_PACKAGE(Qt5Core REQUIRED)
	FIND_PACKAGE(Qt5Gui REQUIRED)
	FIND_PACKAGE(Qt5Widgets REQUIRED)
	FIND_PACKAGE(Qt5OpenGL REQUIRED)
	get_target_property(QT_UIC_EXECUTABLE Qt5::uic LOCATION)
	SET(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
	SET(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})
ENDIF()
#ADD_DEFINITIONS(-DQT_PLUGIN)

# Find and setup OpenGL.
FIND_PACKAGE(OpenGL REQUIRED)

IF(NOT DEFINED RWS_CXX_FLAGS)
	IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
		SET(RWS_CXX_FLAGS_TMP "-DQT_DEBUG") 
	ELSE ()
		SET(RWS_CXX_FLAGS_TMP "-DQT_NO_DEBUG")
	ENDIF()
	
	SET(RWS_CXX_FLAGS ${RWS_CXX_FLAGS_TMP} 
		CACHE STRING "Change this to force using your own 
					  flags and not those of RobWorkStudio"
	)
ENDIF()

IF(NOT DEFINED RWS_LINKER_FLAGS)	
	SET(RWS_LINKER_FLAGS ""
		CACHE STRING "Change this to force using your own linker
					  flags and not those of RobWorkSutdio"
	)
ENDIF()

# Setup the libraries

IF (RWS_BUILD_WITH_SANDBOX)
  SET(RWS_SANDBOX_LIB rws_sandbox)
  SET(RWS_HAVE_SANDBOX ON)
  #MESSAGE(STATUS "RobWorkStudio: Sandbox ENABLED!")
ELSE ()
  SET(RWS_HAVE_SANDBOX OFF) 
  #MESSAGE(STATUS "RobWorkStudio: Sandbox DISABLED!")
ENDIF ()

# optional compilation of sandbox
IF (RWS_BUILD_WITH_LUA)
    MESSAGE(STATUS "RobWorkStudio: Lua ENABLED!")
    SET(RWSTUDIO_LUA rws_lua_s rws_luaeditor)
ELSE ()
    MESSAGE(STATUS "RobWorkStudio: Lua DISABLED!")
ENDIF ()


# Setup RobWorkStudio include and link directories
SET(ROBWORKSTUDIO_INCLUDE_DIR
    ${RWSTUDIO_ROOT}/src/
    ${RWSTUDIO_ROOT}/ext/qtpropertybrowser/src/
)

SET(ROBWORKSTUDIO_LIBRARY_DIRS
    ${RWSTUDIO_ROOT}/libs/
    ${RWSTUDIO_ROOT}/libs/${CMAKE_BUILD_TYPE}/
)

SET(ROBWORKSTUDIO_LIBRARIES_TMP
    ${RWS_SANDBOX_LIB}
    ${RWSTUDIO_LUA}
    rws
    qtpropertybrowser
  	${QT_LIBRARIES}
)

SET(ROBWORKSTUDIO_LIBRARIES)
FOREACH(l ${ROBWORKSTUDIO_LIBRARIES_TMP})
  UNSET(tmp CACHE)
  FIND_LIBRARY(tmp ${l} PATHS ${ROBWORKSTUDIO_LIBRARY_DIRS})
  IF(tmp)
    LIST(APPEND ROBWORKSTUDIO_LIBRARIES ${tmp})
  ELSE()
    LIST(APPEND ROBWORKSTUDIO_LIBRARIES ${l})
  ENDIF()
ENDFOREACH(l)
 
SET(ROBWORKSTUDIO_VERSION ${RWS_BUILD_WITH_VERSION})
 
 
 
MACRO (RWS_QT4_WRAP_UI outfiles )
QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})

FOREACH (it ${ui_files})
  GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
  GET_FILENAME_COMPONENT(outpath ${it} PATH)
  
  SET(outfile ${CMAKE_CURRENT_SOURCE_DIR}/${outpath}/ui_${outfile}.h)
  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
    COMMAND ${QT_UIC_EXECUTABLE}
    ARGS ${ui_options} -o ${outfile} ${infile}
    MAIN_DEPENDENCY ${infile})
  SET(${outfiles} ${${outfiles}} ${outfile})
ENDFOREACH (it)

ENDMACRO (RWS_QT4_WRAP_UI)

MACRO (QT5_EXTRACT_OPTIONS _qt5_files _qt5_options)
  SET(${_qt5_files})
  SET(${_qt5_options})
  SET(_QT5_DOING_OPTIONS FALSE)
  FOREACH(_currentArg ${ARGN})
    IF ("${_currentArg}" STREQUAL "OPTIONS")
      SET(_QT5_DOING_OPTIONS TRUE)
    ELSE ("${_currentArg}" STREQUAL "OPTIONS")
      IF(_QT5_DOING_OPTIONS) 
        LIST(APPEND ${_qt5_options} "${_currentArg}")
      ELSE(_QT5_DOING_OPTIONS)
        LIST(APPEND ${_qt5_files} "${_currentArg}")
      ENDIF(_QT5_DOING_OPTIONS)
    ENDIF ("${_currentArg}" STREQUAL "OPTIONS")
  ENDFOREACH(_currentArg) 
ENDMACRO (QT5_EXTRACT_OPTIONS)

MACRO (RWS_QT5_WRAP_UI outfiles )
QT5_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})

FOREACH (it ${ui_files})
  GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
  GET_FILENAME_COMPONENT(outpath ${it} PATH)
  
  SET(outfile ${CMAKE_CURRENT_SOURCE_DIR}/${outpath}/ui_${outfile}.h)
  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
    COMMAND ${QT_UIC_EXECUTABLE}
    ARGS ${ui_options} -o ${outfile} ${infile}
    MAIN_DEPENDENCY ${infile})
  SET(${outfiles} ${${outfiles}} ${outfile})
ENDFOREACH (it)

ENDMACRO (RWS_QT5_WRAP_UI)
