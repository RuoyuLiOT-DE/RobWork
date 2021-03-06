###########################################################################
#
# This file is for setting up a build of RobWork. It should NOT be used
# for including RobWork in other projects, for that use FindRobWork.cmake 
# 
#  Requirements:
#  
#  RW_ROOT             - Must be set to the root dir of RobWork
#
#  Following is a list of variables that is set by this script: 
# 
#  ROBWORK_INCLUDE_DIR - Where to find robwork include sub-directory.
#  ROBWORK_LIBRARIES   - List of libraries when using RobWork (includes all libraries that RobWork depends on).
#  ROBWORK_LIBARY_DIRS - List of directories where libraries of RobWork are located. 
#  
#

#
# Check if RW_ROOT path are setup correctly
#
FIND_FILE(RW_ROOT_PATH_TEST RobWorkSetup.cmake ${RW_ROOT}/cmake NO_DEFAULT_PATH)
IF(NOT RW_ROOT_PATH_TEST)
    MESSAGE(SEND_ERROR "RobWork: Path to RobWork root (RW_ROOT) is incorrectly setup! \nRW_ROOT == ${RW_ROOT}")
ENDIF()
MESSAGE(STATUS "RobWork: ROOT - ${RW_ROOT}")

#
# Setup the default include and library dirs for robwork
#
#INCLUDE("${RW_ROOT}/build/RobWorkConfig${CMAKE_BUILD_TYPE}.cmake")

####################################################################
# DEPENDENCIES - REQUIRED
# Check for all dependencies, this adds LIBRARY_DIRS and include dirs that 
# the configuration depends on
#

#
# some of the FIND_PACKAGE modules are located in the build directory 
#
SET(CMAKE_MODULE_PATH ${RW_ROOT}/cmake/Modules ${CMAKE_MODULE_PATH})

#
# include the build specific configuration of RobWork
# 
#INCLUDE("${RW_ROOT}/build/RobWorkConfig${CMAKE_BUILD_TYPE}.cmake")
#LINK_DIRECTORIES(${RW_ARCHIVE_OUT_DIR} ${RW_LIBRARY_OUT_DIR})
#LIST(APPEND CMAKE_LIBRARY_PATH ${RW_LIBRARY_OUT_DIR})

#
# We need the Boost package and some of its components.
# Test libraries are optional and can be compiled from header instead.
#

UNSET(Boost_USE_STATIC_LIBS)
UNSET(Boost_FIND_QUIETLY)
SET(Boost_LIBRARIES_TMP "")
IF(DEFINED UNIX)
  #SET(Boost_USE_STATIC_LIBS ON)
  FIND_PACKAGE(Boost REQUIRED filesystem regex serialization system thread program_options)
  SET(Boost_LIBRARIES_TMP ${Boost_LIBRARIES_TMP} ${Boost_LIBRARIES})  
  # Test libraries are optional
  SET(Boost_FIND_QUIETLY TRUE)

  # On Mac OS only the header only version of boost unit test seems to work for now, needs further investigation
  IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_PACKAGE(Boost COMPONENTS test_exec_monitor unit_test_framework)
    SET(Boost_LIBRARIES_TMP ${Boost_LIBRARIES_TMP} ${Boost_LIBRARIES})
  ENDIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  
  IF(NOT Boost_TEST_EXEC_MONITOR_FOUND OR NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    # header only
    #SET(Boost_USE_STATIC_LIBS OFF)
    #FIND_PACKAGE(Boost COMPONENTS test_exec_monitor unit_test_framework)
    SET(RW_USE_BOOST_STATIC_TEST_LIBS off)
  ELSE()
    # libraries found 
    SET(RW_USE_BOOST_STATIC_TEST_LIBS on)  
  ENDIF()
  
ELSEIF(DEFINED WIN32)
  SET(Boost_USE_STATIC_LIBS ON)
  SET(BOOST_ALL_DYN_LINK OFF)
  FIND_PACKAGE(Boost COMPONENTS filesystem regex serialization system thread program_options)
  SET(Boost_LIBRARIES_TMP ${Boost_LIBRARIES_TMP} ${Boost_LIBRARIES})
  # If static libraries for Windows were not found, try searching again for the shared ones
  IF(NOT Boost_FILESYSTEM_FOUND OR NOT Boost_REGEX_FOUND OR NOT Boost_SERIALIZATION_FOUND OR
     NOT Boost_SYSTEM_FOUND OR NOT Boost_THREAD_FOUND)
    SET(Boost_USE_STATIC_LIBS OFF)
    FIND_PACKAGE(Boost REQUIRED filesystem regex serialization system thread program_options)
    SET(Boost_LIBRARIES_TMP ${Boost_LIBRARIES_TMP} ${Boost_LIBRARIES})
    SET(BOOST_ALL_DYN_LINK ON)
  ENDIF()
  
  # Test libraries are optional
  SET(Boost_USE_STATIC_LIBS ON)
  SET(Boost_FIND_QUIETLY TRUE)
  FIND_PACKAGE(Boost COMPONENTS test_exec_monitor unit_test_framework)
  SET(Boost_LIBRARIES_TMP ${Boost_LIBRARIES_TMP} ${Boost_LIBRARIES})
  # If static libraries for Windows were not found, try searching again for the shared ones
  IF(NOT Boost_TEST_EXEC_MONITOR_FOUND OR NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    #SET(Boost_USE_STATIC_LIBS OFF)
    #FIND_PACKAGE(Boost COMPONENTS test_exec_monitor unit_test_framework)
    SET(RW_USE_BOOST_STATIC_TEST_LIBS off)
  ELSE()
    # libraries found 
    SET(RW_USE_BOOST_STATIC_TEST_LIBS on)  
    
  ENDIF()
ENDIF()
SET(Boost_LIBRARIES ${Boost_LIBRARIES_TMP})

MESSAGE(STATUS "RobWork: Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} found!")

# Print test libraries status
IF(Boost_TEST_EXEC_MONITOR_FOUND AND Boost_UNIT_TEST_FRAMEWORK_FOUND)
  MESSAGE(STATUS "RobWork: Found additional Boost libraries: test_exec_monitor and unit_test_framework")
ELSE()
  # Set necessary directory for disabling linking with test libraries for MSVC
	IF(DEFINED MSVC)
		SET(BOOST_TEST_NO_LIB TRUE)
  ELSE()
    SET(BOOST_TEST_NO_LIB FALSE)
	ENDIF()
ENDIF()


#
# We depend on BLAS and Lapack. These depend on FORTRAN, so we enable that
#
ENABLE_LANGUAGE(CXX)
#ENABLE_LANGUAGE(Fortran)

IF (DEFINED RW_USE_UBLAS_LAPACK)
IF(NOT DEFINED WIN32)
  SET(BLA_STATIC ON)
ENDIF()
#FIND_PACKAGE(BLAS REQUIRED)
#FIND_PACKAGE(LAPACK REQUIRED)
FIND_PACKAGE(BLASLAPACK REQUIRED)

SET(LAPACK_BLAS_LIBRARY_DIRS )
#get_filename_component(BLAS_LIBRARY_DIRS ${BLAS_LIBRARIES} PATH)
#get_filename_component(LAPACK_LIBRARY_DIRS ${LAPACK_LIBRARIES} PATH)

FOREACH(lib IN LISTS LAPACK_LIBRARIES BLAS_LIBRARIES)
    get_filename_component(TMP_DIR ${lib} PATH)
    LIST(APPEND LAPACK_BLAS_LIBRARY_DIRS ${TMP_DIR})
ENDFOREACH(lib)
ENDIF()

####################################################################
# DEPENDENCIES - OPTIONAL
# these dependencies are optional, which is the user can switch off
# modules

#
# For some libs we need the opengl package, though it is OPTIONAL 
#
FIND_PACKAGE(OpenGL)
INCLUDE(CMakeDependentOption)
#
# For some of the xml parsing we need xerces, though it is OPTIONAL
#
SET(RW_HAVE_XERCES False)
FIND_PACKAGE(XercesC QUIET)
IF( XERCESC_FOUND )
    SET(RW_HAVE_XERCES True)
    MESSAGE(STATUS "RobWork: XERCES ENABLED! FOUND!")
ELSE ()
    MESSAGE(STATUS "RobWork: Xerces NOT FOUND! Xerces code disabled! (Check if XERCESC_ROOT or XERCESC_INCLUDE_DIR and XERCESC_LIB_DIR is set correctly if you need it)!")
ENDIF ()

#
# If the user wants to use yaobi then search for it, OPTIONAL
#
SET(RW_HAVE_YAOBI False)
CMAKE_DEPENDENT_OPTION(RW_USE_YAOBI "Set to ON to include Yaobi support.
                Set YAOBI_INCLUDE_DIR and YAOBI_LIBRARY_DIR 
                to specify your own YAOBI else RobWork YAOBI will 
                be used!"
      ON "NOT RW_DISABLE_YAOBI" OFF)
IF(RW_USE_YAOBI)
    FIND_PACKAGE(Yaobi QUIET)
    IF( YAOBI_FOUND )
        MESSAGE(STATUS "RobWork: Yaobi ENABLED! FOUND!")
        SET(RW_HAVE_YAOBI True)
    ELSE ()
        SET(RW_ENABLE_INTERNAL_YAOBI_TARGET ON)
        MESSAGE(STATUS "RobWork: Yaobi ENABLED! NOT FOUND! Using RobWork native Yaobi.")
        SET(YAOBI_INCLUDE_DIR "${RW_ROOT}/ext/rwyaobi")
        SET(YAOBI_LIBRARIES "yaobi")
        SET(YAOBI_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
        SET(RW_HAVE_YAOBI True)
        
    ENDIF ()
ELSE ()
    MESSAGE(STATUS "RobWork: Yaobi DISABLED!")
    SET(YAOBI_INCLUDE_DIR "")
ENDIF()

#
# If the user wants to use PQP then search for it or use the default
#
SET(RW_HAVE_PQP False)
CMAKE_DEPENDENT_OPTION(RW_USE_PQP "Set to ON to include PQP support.
                    RobWork PQP will allways be used!" 
    ON "NOT RW_DISABLE_PQP" OFF
)
IF(RW_USE_PQP)
    SET(RW_ENABLE_INTERNAL_PQP_TARGET ON)
    MESSAGE(STATUS "RobWork: PQP ENABLED! Using RobWork native PQP.")
    SET(PQP_INCLUDE_DIR "${RW_ROOT}/ext/rwpqp")
    SET(PQP_LIBRARIES "pqp")
    SET(PQP_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
    SET(RW_HAVE_PQP True)
ELSE ()
    MESSAGE(STATUS "RobWork: PQP DISABLED!")   
    SET(PQP_INCLUDE_DIR "")
ENDIF()

#
# If the user wants to use FCL then search for it, OPTIONAL
#
SET(RW_HAVE_FCL False)
CMAKE_DEPENDENT_OPTION(RW_USE_FCL "Set to ON to include FCL support." ON "NOT RW_DISABLE_FCL" OFF)
IF(RW_USE_FCL)
  FIND_PACKAGE(FCL QUIET)
  IF(FCL_FOUND)
    MESSAGE(STATUS "RobWork: Native FCL installation FOUND! - version ${FCL_VERSION}")
    SET(RW_HAVE_FCL True)
  ELSE()
    SET(RW_ENABLE_INTERNAL_FCL_TARGET ON)
    INCLUDE(${RW_ROOT}/ext/fcl/fcl/CMakeModules/FCLVersion.cmake)
    MESSAGE(STATUS "RobWork: native FCL installation NOT FOUND! Using RobWork ext FCL ${FCL_VERSION}.")
    SET(FCL_INCLUDE_DIRS "${RW_ROOT}/ext/fcl/fcl/include")
    SET(FCL_LIBRARIES "fcl")
    SET(FCL_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
    SET(RW_HAVE_FCL TRUE)
  ENDIF()
ELSE()
  MESSAGE(STATUS "RobWork: FCL DISABLED!")
ENDIF()

#FIND_PACKAGE(Eigen3 3.1.0 QUIET)
FIND_PACKAGE(Eigen3 QUIET)
IF( EIGEN3_FOUND )
    MESSAGE(STATUS "RobWork: EIGEN3 installation FOUND! - version ${EIGEN3_VERSION}")
	IF(EIGEN3_VERSION VERSION_LESS 3.1.0)
		# We need to add this to enable compilation on default ubuntu 12.04 eigen
		# (only for Eigen versions lower than 3.1.0)
		ADD_DEFINITIONS("-DEIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET=1")
	ENDIF()
ELSE ()
    SET(RW_ENABLE_INTERNAL_EIGEN_TARGET ON)
    MESSAGE(STATUS "RobWork: EIGEN3 installation NOT FOUND! Using RobWork ext EIGEN3.")
    SET(EIGEN3_INCLUDE_DIR "${RW_ROOT}/ext/eigen3")
ENDIF ()


# find package disabled, in order to use reentrant qhull
#FIND_PACKAGE(Qhull QUIET)
#IF( QHULL_FOUND )
#    MESSAGE(STATUS "RobWork: QHULL installation FOUND!")
#ELSE ()
    SET(RW_ENABLE_INTERNAL_QHULL_TARGET ON)
    MESSAGE(STATUS "RobWork: QHULL installation NOT FOUND! Using RobWork ext QHULL.")
    
    SET(QHULL_INCLUDE_DIRS "${RW_ROOT}/ext/qhull/src")
    SET(QHULL_LIBRARIES "rw_qhull")
    SET(QHULL_DEFINITIONS "")
#ENDIF ()

# CSGJS
MESSAGE(STATUS "Using CsgJs.")
SET(CSGJS_INCLUDE_DIRS "${RW_ROOT}/ext/csgjs/src")
SET(CSGJS_LIBRARIES "rw_csgjs")
SET(CSGJS_DEFINITIONS "")

Find_Package(Bullet)
SET(RW_HAVE_BULLET FALSE)
IF( BULLET_FOUND )
	MESSAGE("Bullet found! ${BULLET_INCLUDE_DIRS}")
	MESSAGE("Bullet libs: ${BULLET_LIBRARIES}")
   #SET(RW_HAVE_BULLET TRUE)
   
   #INCLUDE_DIRECTORIES( ${BULLET_INCLUDE_DIRS} )
   SET(BULLET_LIBRARIES "")
   SET(BULLET_INCLUDE_DIRS "") 
ELSE()
  SET(BULLET_INCLUDE_DIRS "")
ENDIF()


#
# If the user wants to use LUA then search for it or use the default
#
SET(RW_HAVE_LUA False)
SET(RW_HAVE_SWIG False)

FIND_PACKAGE(SWIG 3.0.0 QUIET) # At least SWIG 3 to support C++11
CMAKE_DEPENDENT_OPTION(RW_USE_LUA "Set to ON to include PQP support.
                Set PQP_INCLUDE_DIR and PQP_LIB_DIR 
                to specify your own PQP else RobWork PQP will 
                be used!" 
    ON "SWIG_FOUND;NOT RW_DISABLE_LUA" OFF)
    
IF(RW_USE_LUA)
	SET(RW_FOUND_LUA FALSE)
	IF(${CMAKE_VERSION} VERSION_LESS "3.0.0")
    	FIND_PACKAGE(Lua51 QUIET)
    	IF( LUA51_FOUND )
        	MESSAGE(STATUS "RobWork: External lua ${LUA_VERSION_STRING} FOUND!")
        	SET (RW_FOUND_LUA TRUE)
        ENDIF()
    ELSE()
    	FIND_PACKAGE(Lua QUIET)
    	IF( LUA_FOUND )
    		IF (LUA_VERSION_MAJOR GREATER "5" OR LUA_VERSION_MINOR GREATER "0")
        		MESSAGE(STATUS "RobWork: External lua ${LUA_VERSION_STRING} FOUND!")
        		SET(RW_FOUND_LUA TRUE)
        	ENDIF()
    	ENDIF ()
	ENDIF()
    IF(NOT RW_FOUND_LUA)
       	SET(RW_ENABLE_INTERNAL_LUA_TARGET ON)
       	MESSAGE(STATUS "RobWork:  External lua NOT FOUND! Using RobWork native Lua.")
       	SET(LUA_INCLUDE_DIR "${RW_ROOT}/ext/lua/src/")
       	SET(LUA_LIBRARIES "lua51")
       	SET(LUA_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
    ENDIF ()
    UNSET(RW_FOUND_LUA)

    IF( SWIG_FOUND )
        MESSAGE(STATUS "RobWork: LUA ENABLED! Both SWIG ${SWIG_VERSION} and Lua FOUND!")
        SET(RW_HAVE_SWIG True)
        SET(RW_HAVE_LUA True)
        set(RW_LUA_LIBS rw_lua_s)
    ELSE ()
        SET(RW_HAVE_SWIG False)
        SET(RW_HAVE_LUA False)
        MESSAGE(SEND_ERROR "RobWork: Lua DISABLED! Since SWIG 3+ was NOT FOUND!")
    ENDIF ()
ELSE ()
    IF(SWIG_FOUND)
        
    ELSE()
        MESSAGE(STATUS "RobWork: LUA DISABLED! Swig 3+ not found!")
    ENDIF()
    SET(LUA_INCLUDE_DIR "")
    #SET(TOLUA_INCLUDE_DIR "")
ENDIF()
  
IF (RW_BUILD_SANDBOX)
    MESSAGE(STATUS "RobWork: Sandbox ENABLED!")
    SET(SANDBOX_LIB "rw_sandbox")
ELSE ()
    MESSAGE(STATUS "RobWork: Sandbox DISABLED!")    
ENDIF ()


#
# If the user wants to use the softbody package
#

OPTION( RW_BUILD_SOFTBODY "Set when you want to build softbody module" OFF )
IF ( RW_BUILD_SOFTBODY )
    MESSAGE( STATUS "RobWork: Softbody ENABLED!" )
    # Make sure to have set environment variable, e.g. in .bashrc
    # export IPOPT_HOME=/home/arf/Documents/Ipopt-3.10.3
    FIND_PACKAGE( MUMPS REQUIRED )
    FIND_PACKAGE( IPOPT REQUIRED )
    SET( SOFTBODY_LIBRARY_DIRS ${IPOPT_LIBRARY_DIRS} ${MUMPS_LIBRARY_DIRS})
    SET( SOFTBODY_LIB rw_softbody  ${MUMPS_LIBRARIES} ${IPOPT_LIBRARIES})
    SET( SOFTBODY_INCLUDE_DIRS  ${MUMPS_INCLUDE_DIRS} ${IPOPT_INCLUDE_DIRS})
ELSE ()
    MESSAGE( STATUS "RobWork: Softbody DISABLED!" )    
ENDIF()

#
# If the user wants to use the Assimp package then search for it or build internal Assimp.
# Set RW_DISABLE_ASSIMP to ON to disable Assimp completely.
# Zlib and Minizip/Unzip will be found/build when Assimp is enabled.
#

SET(RW_HAVE_ZLIB FALSE)
SET(RW_HAVE_MINIZIP FALSE)
SET(RW_HAVE_ASSIMP FALSE)

# Make option for user to disable Assimp
CMAKE_DEPENDENT_OPTION(RW_USE_ASSIMP "Set to ON to include Assimp support.
                Set ASSIMP_INCLUDE_DIR and ASSIMP_LIBRARY_DIR 
                to specify your own Assimp else RobWork Assimp will 
                be used!"
      ON "NOT RW_DISABLE_ASSIMP" OFF)

SET(ASSIMP_INCLUDE_DIRS "")
SET(ASSIMP_LIBRARIES "")
IF(RW_USE_ASSIMP)
	# Now try to find Assimp
	FIND_PACKAGE(Assimp 3.0 QUIET)
	IF( ASSIMP_FOUND )
		MESSAGE(STATUS "RobWork: Native Assimp installation FOUND!")
		SET(RW_HAVE_ASSIMP TRUE)
	ELSE ()
		SET(RW_ENABLE_INTERNAL_ASSIMP_TARGET ON)
		MESSAGE(STATUS "RobWork: Assimp 3.0 installation NOT FOUND! Using RobWork ext Assimp.")

		SET(ASSIMP_INCLUDE_DIRS "${RW_ROOT}/ext/assimp/include")
		SET(ASSIMP_LIBRARIES "rw_assimp")
        	SET(ASSIMP_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
		SET(RW_HAVE_ASSIMP TRUE)

		# Find Zlib
		IF(NOT RW_HAVE_ZLIB)
    			FIND_PACKAGE(ZLIB QUIET)
			IF( ZLIB_FOUND )
				MESSAGE(STATUS "RobWork: Native ZLIB FOUND")
			ELSE ()
				MESSAGE(STATUS "RobWork: No ZLIB FOUND - using internal")
				SET(RW_ENABLE_INTERNAL_ZLIB_TARGET ON)
    				SET(ZLIB_INCLUDE_DIRS "${RW_ROOT}/ext/zlib")
        			SET(ZLIB_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
	    			SET(ZLIB_LIBRARIES "rw_zlib")
			ENDIF()
			SET(RW_HAVE_ZLIB ON)
		ENDIF (NOT RW_HAVE_ZLIB)

		# Find Minizip/Unzip
		IF(NOT RW_HAVE_MINIZIP)
	    		FIND_PACKAGE(MINIZIP QUIET)
			IF( MINIZIP_FOUND )
				MESSAGE(STATUS "RobWork: Native MINIZIP FOUND")
			ELSE ()
				MESSAGE(STATUS "RobWork: No MINIZIP FOUND - using internal")
				SET(RW_ENABLE_INTERNAL_MINIZIP_TARGET ON)
	    			SET(MINIZIP_INCLUDE_DIRS "${RW_ROOT}/ext/unzip")
	        		SET(MINIZIP_LIBRARY_DIRS ${RW_LIBRARY_OUT_DIR})
		    		SET(MINIZIP_LIBRARIES "rw_unzip")
			ENDIF()
			SET(RW_HAVE_MINIZIP ON)
		ENDIF (NOT RW_HAVE_MINIZIP)
	
	ENDIF()
ELSE ()
    MESSAGE( STATUS "RobWork: Assimp DISABLED!" )
ENDIF ()

#
# If the user wants to use the Google Test package then search for it.
# Set RW_DISABLE_GTEST to ON to disable Google Test completely.
#

# Make option for user to disable Google Test
CMAKE_DEPENDENT_OPTION(RW_USE_GTEST "Set to ON to include Google Test support.
                Set GTEST_ROOT or GTEST_SOURCE to specify your own Google Test installation."
      ON "NOT RW_DISABLE_GTEST" OFF)
IF(RW_USE_GTEST)
	# Now try to find Google Test
	SET(gtest_force_shared_crt ON CACHE BOOL "Use /MD on Windows systems.")
	FIND_PACKAGE(GTest QUIET)
	IF( GTEST_FOUND )
		SET(GTEST_SHARED_LIBS ${BUILD_SHARED_LIBS})
		MESSAGE(STATUS "RobWork: Google Test installation FOUND!")
		SET(RW_HAVE_GTEST TRUE)
	ELSE()
		MESSAGE(WARNING "RobWork: Google Test installation NOT FOUND!")
	ENDIF()
ELSE ()
    MESSAGE( STATUS "RobWork: Google Test DISABLED!" )
ENDIF ()

# Mathematica
CMAKE_DEPENDENT_OPTION(RW_USE_MATHEMATICA "Set to ON to include Mathematica support."
      ON "RW_ENABLE_MATHEMATICA" OFF)
IF(RW_USE_MATHEMATICA)
	FIND_PACKAGE(Mathematica QUIET)
	IF( Mathematica_WSTP_FOUND )
		MESSAGE(STATUS "RobWork: Mathematica WSTP installation FOUND!")
		FOREACH(math_lib_dirs ${Mathematica_LIBRARY_DIRS})
			IF (${math_lib_dirs} MATCHES "/Libraries/")
				SET (UUID_LIB_DIR ${math_lib_dirs})
			ENDIF()
		ENDFOREACH()
		IF (DEFINED UUID_LIB_DIR)
			SET(Mathematica_WSTP_LIBRARIES ${Mathematica_WSTP_LIBRARIES} ${UUID_LIB_DIR}/libuuid.a)
		ENDIF()
		SET(RW_MATHEMATICA_LIB rw_mathematica)
		SET(RW_HAVE_MATHEMATICA TRUE)
	ELSE ()
    	MESSAGE( STATUS "RobWork: Mathematica NOT FOUND!" )
		SET(RW_HAVE_MATHEMATICA FALSE)
	ENDIF()
ELSE ()
    MESSAGE( STATUS "RobWork: Mathematica DISABLED!" )
	SET(RW_HAVE_MATHEMATICA FALSE)
ENDIF ()

#######################################################################
# COMPILER FLAGS AND MACRO SETUP
#

#
# Enable the RW_ASSERT() macro.
#
OPTION(RW_ENABLE_ASSERT "Enables RW_ASSERT macro: on|off" ON)
IF( RW_ENABLE_ASSERT )
    MESSAGE(STATUS "RobWork: RW_ASSERT enabled.")
    ADD_DEFINITIONS(-DRW_ENABLE_ASSERT)
ELSE ()
    MESSAGE(STATUS "RobWork: RW_ASSERT disabled.")
ENDIF ()

RW_IS_RELEASE(IS_RELEASE)
#
# Set extra compiler flags. The user should be able to change this
# 

IF( "${RW_C_FLAGS}" STREQUAL "")
    # GCC and MinGW
    IF ( (CMAKE_COMPILER_IS_GNUCC) OR (CMAKE_C_COMPILER_ID STREQUAL "Clang") )
      # Necessary Linux-GCC flag
      IF(DEFINED UNIX)
        SET(RW_C_FLAGS_TMP "${RW_C_FLAGS_TMP} -fPIC")
      ENDIF()
    ENDIF ()

	IF(DEFINED RW_C_FLAGS_EXTRA)
	  SET(RW_C_FLAGS_TMP "${RW_C_FLAGS_TMP} ${RW_C_FLAGS_EXTRA}")
	ENDIF()
        
	SET(RW_C_FLAGS "${RW_C_FLAGS_TMP}"
		CACHE STRING "Change this to force using your own 
					  flags and not those of RobWork" FORCE
	)
ENDIF()

IF( "${RW_CXX_FLAGS}" STREQUAL "")
    # GCC and MinGW
    IF ( (CMAKE_COMPILER_IS_GNUCXX) OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") )
      # Turn off annoying GCC warnings
      SET(RW_CXX_FLAGS_TMP "-Wall -Wno-strict-aliasing -Wno-unused-function -Wno-pragmas")
      IF ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
      	SET(RW_CXX_FLAGS_TMP "-Wall -Wno-strict-aliasing -Wno-unused-function")
      ENDIF()
      
      # Necessary Linux-GCC flag
      IF(DEFINED UNIX)
        SET(RW_CXX_FLAGS_TMP "${RW_CXX_FLAGS_TMP} -fPIC")
      ENDIF()
    ENDIF ()
    
    # Setup crucial MSVC flags, without these RobWork does not compile
    IF (DEFINED MSVC)
      SET(RW_CXX_FLAGS_TMP "-EHa -bigobj /MP")
    ENDIF ()

	# Set C++11 standard (except if user has specified this explicitly in the RW_CXX_FLAGS_EXTRA variable).
	SET(RW_CXX_FLAGS_SET_STD FALSE)
	IF(CMAKE_COMPILER_IS_GNUCXX)
		IF(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.1.0") # from GNU 6.1 gnu++14 should be the default
			SET(RW_CXX_FLAGS_SET_STD TRUE)
			FOREACH(flag ${RW_CXX_FLAGS_EXTRA})
				STRING(REGEX MATCH ".*-std=.*" flag ${flag})
				IF(flag)
					SET(RW_CXX_FLAGS_SET_STD FALSE)
				ENDIF()
			ENDFOREACH()
		ENDIF()
	ENDIF()
	IF(RW_CXX_FLAGS_SET_STD)
		SET(RW_CXX_FLAGS_TMP "${RW_CXX_FLAGS_TMP} -std=c++11")
	ENDIF()

	IF(DEFINED RW_CXX_FLAGS_EXTRA)
	  SET(RW_CXX_FLAGS_TMP "${RW_CXX_FLAGS_TMP} ${RW_CXX_FLAGS_EXTRA}")
	ENDIF()
        
	SET(RW_CXX_FLAGS "${RW_CXX_FLAGS_TMP}"
		CACHE STRING "Change this to force using your own 
					  flags and not those of RobWork" FORCE
	)
ENDIF()

#
# Enable the use of OMP definitions.
#
OPTION(RW_ENABLE_OMP "Enables use of OpenMP #pragmas: on|off" ON)
IF( RW_ENABLE_OMP )
    MESSAGE(STATUS "RobWork: OpenMP enabled.")
    FIND_PACKAGE(OpenMP QUIET)
    IF(${CMAKE_VERSION} VERSION_LESS "3.9")
		IF( OPENMP_FOUND )
			IF(${CMAKE_VERSION} VERSION_LESS "3.7")
				MESSAGE(STATUS "RobWork: OpenMP CXX FOUND!")
			ELSE()
				MESSAGE(STATUS "RobWork: OpenMP CXX FOUND! - Specification date ${OpenMP_CXX_SPEC_DATE}")
			ENDIF()
			SET(RW_HAVE_OMP TRUE)
			SET(RW_CXX_FLAGS "${RW_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		ELSE ()
			MESSAGE( STATUS "RobWork: OpenMP CXX NOT FOUND!" )
			SET(RW_HAVE_OMP FALSE)
		ENDIF()
    ELSE() # CMake 3.9 and newer
		IF( OpenMP_CXX_FOUND )
			MESSAGE(STATUS "RobWork: OpenMP ${OpenMP_CXX_VERSION} CXX FOUND! - Specification date ${OpenMP_CXX_SPEC_DATE}")
			SET(RW_HAVE_OMP TRUE)
			SET(RW_CXX_FLAGS "${RW_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
			# Todo: use OpenMP_CXX_LIB_NAMES, OpenMP_CXX_LIBRARY and/or OpenMP_CXX_LIBRARIES ?
		ELSE ()
			MESSAGE( STATUS "RobWork: OpenMP CXX NOT FOUND!" )
			SET(RW_HAVE_OMP FALSE)
		ENDIF()
	ENDIF()
ELSE ()
    MESSAGE(STATUS "RobWork: OpenMP disabled.")
ENDIF ()

IF( "${RW_DEFINITIONS}" STREQUAL "")
    # GCC and MinGW
    IF ( (CMAKE_COMPILER_IS_GNUCXX) OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") )
      SET(RW_DEFINITIONS_TMP)

      IF(IS_RELEASE)
          LIST(APPEND RW_DEFINITIONS_TMP "-DBOOST_DISABLE_ASSERTS")
      ENDIF()

      IF(DEFINED MINGW AND AMD64)
        LIST(APPEND RW_DEFINITIONS_TMP "-DBOOST_USE_WINDOWS_H")
      ENDIF()
    ENDIF ()
    
    # Setup crucial MSVC flags, without these RobWork does not compile
    IF (DEFINED MSVC)
      SET(RW_DEFINITIONS_TMP # Remove the min()/max() macros or else RobWork won't compile.
                           "-DNOMINMAX" 
                           # Without this define for boost-bindings we can't link with lapack.
                           "-DBIND_FORTRAN_LOWERCASE_UNDERSCORE"
						   "-DWIN32_LEAN_AND_MEAN"
						   "-D_WIN32_WINNT=0x0501"
                           "-D_SCL_SECURE_NO_WARNINGS"
                           "-D_CRT_SECURE_NO_WARNINGS"
                           "-D_CRT_SECURE_NO_DEPRECATE"
      )

      IF(BOOST_TEST_NO_LIB)
        LIST(APPEND RW_DEFINITIONS_TMP "-DBOOST_TEST_NO_LIB")
      ENDIF()
      IF(BOOST_ALL_DYN_LINK)
        LIST(APPEND RW_DEFINITIONS_TMP "-DBOOST_ALL_DYN_LINK")
      ENDIF()

      # Current issues addressed for MSVC 64 bit:
      # 	- MSVC 64-bit does not support __asm keyword which is used by default in Yaobi.
      # 	  Therefore, we only define YAOBI_USE_FCOMI in ext/yaobi/yaobi_settings.h for 32 bit architectures.
      IF(AMD64)
        LIST(APPEND RW_DEFINITIONS_TMP "-DMSVC_AMD64")
      ENDIF()
    ENDIF ()

	# Set necessary options for Win32 environments if static version of Xerces is used
	IF(RW_HAVE_XERCES AND XERCES_USE_STATIC_LIBS)
		LIST(APPEND RW_DEFINITIONS_TMP "-DXERCES_STATIC_LIBRARY")
	ENDIF()

	IF(DEFINED RW_DEFINITIONS_EXTRA)
	  SET(RW_DEFINITIONS_EXTRA_TMP "${RW_DEFINITIONS_EXTRA_TMP} ${RW_DEFINITIONS_EXTRA_EXTRA}")
	ENDIF()

	SET(RW_DEFINITIONS "${RW_DEFINITIONS_TMP}"
		CACHE STRING "Change this to force using your own 
					  flags and not those of RobWork" FORCE
	)
ENDIF()

IF( "${RW_CXX_FLAGS}" STREQUAL "")
	MESSAGE(WARNING "Something might be wrong. No CXX FLAGS have been specified. You may be using an unsupported compiler!!")
ENDIF()

ADD_DEFINITIONS(${RW_DEFINITIONS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RW_CXX_FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RW_C_FLAGS}")
MESSAGE(STATUS "RobWork: RW C flags: ${RW_C_FLAGS}")
MESSAGE(STATUS "RobWork: RW CXX flags: ${RW_CXX_FLAGS}")
MESSAGE(STATUS "RobWork: RW definitions: ${RW_DEFINITIONS}")

#
# Set extra linker flags. The user should be able to change this
#
IF(NOT DEFINED RW_LINKER_FLAGS)
	# Set necessary linker options for Win32 environments if static version of Xerces is used
	IF(RW_HAVE_XERCES)
		IF(MSVC AND XERCES_USE_STATIC_LIBS)
			IF(NOT IS_RELEASE)
				SET(RW_LINKER_FLAGS "/NODEFAULTLIB:LIBCMTD")
			ELSE()
				SET(RW_LINKER_FLAGS "/NODEFAULTLIB:LIBCMT")
			ENDIF()
		ENDIF()
	ENDIF()
ENDIF()
SET(RW_BUILD_WITH_LINKER_FLAGS "${RW_LINKER_FLAGS}") 
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${RW_LINKER_FLAGS}" CACHE STRING "" FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${RW_LINKER_FLAGS}" CACHE STRING "" FORCE)
IF(WIN32)
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${RW_LINKER_FLAGS}" CACHE STRING "" FORCE)
ENDIF()
MESSAGE(STATUS "RobWork: RW linker flags: ${RW_LINKER_FLAGS}")

#MESSAGE(" ${Boost_MAJOR_VERSION} ${Boost_MINOR_VERSION} ")
IF(${Boost_MINOR_VERSION} VERSION_LESS 41 ) 
    # proerty tree is not included in earlier versions 1.41 of boost
    MESSAGE(FATAL_ERROR "RobWork: Boost ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} found, no support for property_tree. Please choose Boost version 1.41 or newer!")   
ENDIF()

IF(${Boost_MINOR_VERSION} VERSION_LESS 44 )
    ADD_DEFINITIONS("-DBOOST_FILESYSTEM_VERSION=2")
ELSEIF(${Boost_MINOR_VERSION} VERSION_LESS 46) # version 3 is the default for Boost 1.46 and later
    ADD_DEFINITIONS("-DBOOST_FILESYSTEM_VERSION=3")
ENDIF()

IF(MSVC)
	ADD_DEFINITIONS("-DEIGEN_DONT_ALIGN_STATICALLY=1")
ENDIF()



###########################################################################
# SETTING UP VARS
# here we setup the output variables
# 

#
# The include dirs
#
SET(ROBWORK_INCLUDE_DIR
    # todo: we should actually search for an installation of boostbinding.. instead of allways using the one in RobWork
    ${RW_ROOT}/ext/boostbindings
    ${EIGEN3_INCLUDE_DIR}
    ${SOFTBODY_INCLUDE_DIRS}
    ${ADDITIONAL_BOOST_BINDINGS}
    ${RW_ROOT}/src
    ${OPENGL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${YAOBI_INCLUDE_DIR}
    ${PQP_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${TOLUA_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIRS}
    ${QHULL_INCLUDE_DIRS}
    ${CSGJS_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
#    ${MINIZIP_INCLUDE_DIRS} # Do not include this overall as there is a conflict with another crypt.h that Python includes.
    ${ASSIMP_INCLUDE_DIRS}
    ${Mathematica_WSTP_INCLUDE_DIR}
)

#
# The library dirs
#
SET(ROBWORK_LIBRARY_DIRS
    ${SOFTBODY_LIBRARY_DIRS}
    ${RW_CMAKE_LIBRARY_OUTPUT_DIRECTORY} 
    ${RW_CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    ${Boost_LIBRARY_DIRS}
    ${XERCESC_LIB_DIR}
    ${YAOBI_LIBRARY_DIRS}
    ${PQP_LIBRARY_DIRS}
    ${FCL_LIBRARY_DIRS}
    ${LUA_LIBRARY_DIRS}
    ${BULLET_LIBRARY_DIRS}
    ${TOLUA_LIBRARY_DIRS}
    ${LAPACK_BLAS_LIBRARY_DIRS}
    ${ZLIB_LIBRARY_DIRS}
    ${MINIZIP_LIBRARY_DIRS}
    ${ASSIMP_LIBRARY_DIRS}
    ${Mathematica_WSTP_INCLUDE_DIR}
)



#
# Setup the Library List here. We need to make sure the correct order is maintained
# which is crucial for some compilers.
# 
SET(ROBWORK_LIBRARIES_TMP
  ${SANDBOX_LIB}
  ${RW_LUA_LIBS}
  ${LUA_LIBRARIES}
  rw_algorithms
  rw_pathplanners
  rw_pathoptimization
  rw_simulation
  rw_opengl
  rw_assembly
  rw_task
  rw_calibration
  rw_csg
  rw_control
  ${RW_MATHEMATICA_LIB}
  rw_proximitystrategies
  ${YAOBI_LIBRARIES}
  ${PQP_LIBRARIES}
  ${FCL_LIBRARIES}
  rw
  ${SOFTBODY_LIB}
  ${OPENGL_LIBRARIES}
  ${XERCESC_LIBRARIES}
  ${BULLET_LIBRARIES}
  ${ASSIMP_LIBRARIES}
  ${Boost_LIBRARIES}
  ${LAPACK_LIBRARIES} 
  ${BLAS_LIBRARIES}
  ${QHULL_LIBRARIES}
  ${CSGJS_LIBRARIES}
  ${MINIZIP_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${Mathematica_WSTP_LIBRARIES}
)

SET(ROBWORK_LIBRARIES)
FOREACH(l ${ROBWORK_LIBRARIES_TMP})
  UNSET(tmp CACHE)
  FIND_LIBRARY(tmp ${l} PATHS ${ROBWORK_LIBRARY_DIRS} NO_DEFAULT_PATH)
  IF(tmp)
    LIST(APPEND ROBWORK_LIBRARIES ${tmp})
  ELSE()
    LIST(APPEND ROBWORK_LIBRARIES ${l})
  ENDIF()
ENDFOREACH(l)
