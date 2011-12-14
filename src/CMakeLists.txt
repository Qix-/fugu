cmake_minimum_required(VERSION 2.8)
project(FG)

set(BASE_DIR ${FG_SOURCE_DIR}/..) 

if (WIN32)
	set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/fugu_win)
else(WIN32)
	set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/fugu_osx)
endif(WIN32)

set (FG_VERSION_MAJOR 0)
set (FG_VERSION_MINOR 2)
set (FG_VERSION_PATCH 1)
execute_process(COMMAND hg tip --template "{node|short}" OUTPUT_VARIABLE FG_VERSION_BUILD_HASH)

set (FG_BASE_LOCATION "./")
set (FG_SCRIPTS_LOCATION "scripts/")
set (FUGU_URL "http://bp.io/fugu/")

configure_file (
  "${FG_SOURCE_DIR}/fg_config.h.in"
  "${FG_BINARY_DIR}/fg_config.h"
  )
	
if(WIN32)
	# set(CMAKE_CXX_FLAGS "-D LUABIND_DYNAMIC_LINK") 
	# MAYBE NEED THIS FOR VCG/GL stuff?!	
	# set(CMAKE_CXX_FLAGS "-pg -D GLEW_STATIC") #enable PROFILING
	set(CMAKE_CXX_FLAGS " -O3 -D GLEW_STATIC -Wno-deprecated")
	SET(GL_LIBS glew32s glfw opengl32 glu32)
	SET(LUA_LIBS lua luabind)
	SET(BOOST_LIBS boost_filesystem-mgw45-mt-1_46_1 boost_system-mgw45-mt-1_46_1)
	link_directories(${BASE_DIR}/lib/msw) 
endif(WIN32)

if(APPLE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -O3 -D GLEW_STATIC")
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	#find_package(Boost COMPONENTS system filesystem REQUIRED)
	#include_directories(${Boost_INCLUDE_DIRS})
	FIND_PACKAGE(OpenGL)
	include_directories(${OPENGL_INCLUDES})
	link_directories(${BASE_DIR}/lib/osx ${Boost_LIBRARY_DIRS})
	SET(GL_LIBS GLEW GLFWd ${COCOA_LIBRARY} ${OPENGL_LIBRARIES})
	#ADD_LIBRARY(luabind SHARED IMPORTED)
	#SET(IMPORTED_LOCATION ${BASE_DIR}/lib/osx)
	#SET_PROPERTY(TARGET luabind PROPERTY IMPORTED_LOCATION ${BASE_DIR}/lib/osx/libluabind.dylib)
	SET(LUA_LIBS lua5.1 luabind)
	#SET(BOOST_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}) 
	SET(BOOST_LIBS boost_filesystem boost_system)

	# use, i.e. don't skip the full RPATH for the build tree
	#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)	
	# when building, don't use the install RPATH already
	# (but later on when installing)
	#SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)	
	#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")	
	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)	
	# the RPATH to be used when installing, but only if it's not a system directory
	#LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
	#IF("${isSystemDir}" STREQUAL "-1")
	#   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
	#ENDIF("${isSystemDir}" STREQUAL "-1")
endif(APPLE)

# dependencies
include_directories(${BASE_DIR}/include)
include_directories(${BASE_DIR}/include/vcg)
include_directories(${BASE_DIR}/include/lua)
include_directories("${FG_BINARY_DIR}")

# fg includes
include_directories(${FG_SOURCE_DIR})

add_subdirectory(fg)
add_subdirectory(fugu)

  
