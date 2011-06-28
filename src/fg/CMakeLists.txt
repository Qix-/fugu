add_library(fg 
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
	fg.h)

target_link_libraries(fg ${LUA_LIBS} ${GL_LIBS})
