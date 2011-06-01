cmake_minimum_required(VERSION 2.8)
project(FG)

set(BASE_DIR ${FG_SOURCE_DIR}) 

if(WIN32)
	#MAYBE NEED THIS FOR VCG/GL stuff?!
	set(CMAKE_CXX_FLAGS "-D GLEW_STATIC")
	SET(GL_LIBS glew32s glfw opengl32 glu32)
	SET(LUA_LIBS lua luabind)
	SET(BOOST_LIBS 
		libboost_filesystem-mgw45-mt-1_46_1
		libboost_system-mgw45-mt-1_46_1)
	link_directories(${BASE_DIR}/lib/msw) 
endif(WIN32)

if(APPLE)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	find_package(Boost COMPONENTS system filesystem REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
	FIND_PACKAGE(OpenGL)
	include_directories(${OPENGL_INCLUDES})
	link_directories(${BASE_DIR}/lib/osx ${Boost_LIBRARY_DIRS})
	SET(GL_LIBS GLEW GLFW ${COCOA_LIBRARY} ${OPENGL_LIBRARIES})
	SET(LUA_LIBS lua5.1 luabind)
	SET(BOOST_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}) 
endif(APPLE)

include_directories(${BASE_DIR}/include)
include_directories(${BASE_DIR}/include/vcg)
include_directories(${BASE_DIR}/include/lua)

add_subdirectory(src/fg)
# add_subdirectory(src/misc/iso)
add_subdirectory(src/misc/basics)
add_subdirectory(src/misc/vcgtests)
