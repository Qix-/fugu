cmake_minimum_required(VERSION 2.8)
project(ISO)

set(BASE_DIR ${ISO_SOURCE_DIR}/../../..)

include_directories(${BASE_DIR}/include)
include_directories(${BASE_DIR}/include/vcg)
include_directories(${BASE_DIR}/include/lua)

if(WIN32)
	#MAYBE NEED THIS FOR VCG/GL stuff?!
	set(CMAKE_CXX_FLAGS "-D GLEW_STATIC")
	SET(EXTRA_LIBS glew32s glfw opengl32 glu32 lua luabind)
	link_directories(${BASE_DIR}/lib/msw) 
endif(WIN32)

if(APPLE)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_PACKAGE(OpenGL)
	include_directories(${OPENGL_INCLUDES})
	link_directories(${BASE_DIR}/lib/osx) 
	# TODO: Add Lua/LuaBind stuff here...
	SET(EXTRA_LIBS GLEW GLFW ${COCOA_LIBRARY} ${OPENGL_LIBRARIES} lua5.1 luabind)
endif(APPLE)

add_executable(iso iso.cpp)
target_link_libraries(iso ${EXTRA_LIBS})

#add_executable(iso_off iso_off.cpp ${BASE_DIR}/include/vcg/wrap/ply/plylib.cpp)
#target_link_libraries(iso_off ${EXTRA_LIBS})

#add_executable(iso_smooth iso_smooth.cpp)
#target_link_libraries(iso_smooth ${EXTRA_LIBS})

#add_executable(smoothtest smoothtest.cpp)
#target_link_libraries(smoothtest ${EXTRA_LIBS})
