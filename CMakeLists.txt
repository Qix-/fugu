cmake_minimum_required(VERSION 2.8)
project(FG)

set(BASE_DIR ${FG_SOURCE_DIR}) 
set(CMAKE_INSTALL_PREFIX ${FG_SOURCE_DIR}/bin)

ENABLE_TESTING()

if(WIN32)
	#set(CMAKE_CXX_FLAGS "-D LUABIND_DYNAMIC_LINK")
	#MAYBE NEED THIS FOR VCG/GL stuff?!
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -D GLEW_STATIC")
	SET(GL_LIBS glew32s glfw opengl32 glu32)
	SET(LUA_LIBS lua luabind)
	SET(BOOST_LIBS 
		boost_filesystem-mgw45-mt-1_46_1
		boost_system-mgw45-mt-1_46_1)
	link_directories(${BASE_DIR}/lib/msw) 
endif(WIN32)

if(APPLE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -D GLEW_STATIC")
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

# fg includes
include_directories(${BASE_DIR}/src)

add_subdirectory(src/fg)
add_subdirectory(src/fgv)
add_subdirectory(src/fgo)
add_subdirectory(src/misc/basics)
#add_subdirectory(src/misc/james)
#add_subdirectory(src/tests)
if(WIN32) # ben's test code
 add_subdirectory(src/misc/ben)
# add_subdirectory(src/misc/vcgtests)
endif(WIN32)

  
