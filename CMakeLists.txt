cmake_minimum_required(VERSION 2.8)
project(FG)

set(BASE_DIR ${FG_SOURCE_DIR}) 
include_directories(${BASE_DIR}/include)
include_directories(${BASE_DIR}/include/vcg)
include_directories(${BASE_DIR}/include/lua)

if(WIN32)
	#MAYBE NEED THIS FOR VCG/GL stuff?!
	set(CMAKE_CXX_FLAGS "-D GLEW_STATIC")
	SET(GL_LIBS glew32s glfw opengl32 glu32)
	SET(LUA_LIBS lua luabind)
	link_directories(${BASE_DIR}/lib/msw) 
endif(WIN32)

if(APPLE)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_PACKAGE(OpenGL)
	include_directories(${OPENGL_INCLUDES})
	link_directories(${BASE_DIR}/lib/osx)
	SET(GL_LIBS GLEW GLFW ${COCOA_LIBRARY} ${OPENGL_LIBRARIES})
	SET(LUA_LIBS lua5.1 luabind)
endif(APPLE)

add_subdirectory(src/fg)
# add_subdirectory(src/misc/iso)
add_subdirectory(src/misc/basics)
add_subdirectory(src/misc/vcgtests)
