add_library(fg fg.cpp universe.cpp bindings.cpp mesh.cpp vec3.cpp functions.cpp)
target_link_libraries(fg ${LUA_LIBS} ${GL_LIBS})
