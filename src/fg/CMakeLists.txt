set(SRC 
	fg.cpp
	universe.cpp
	bindings.cpp
	mesh.cpp
	vec3.cpp
	functions.cpp
	meshimpl.cpp
	meshoperators.cpp
	meshoperators_vcg.cpp
	proxy.cpp
	vertex.cpp
	glrenderer.cpp
	node.cpp
	mat4.cpp)

FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

add_library(fg ${SRC} ${HEADERS} ${HEADER_INC})
target_link_libraries(fg ${LUA_LIBS} ${GL_LIBS})
