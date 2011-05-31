SET(ALL_LIBS ${LUA_LIBS} ${GL_LIBS})

add_executable(perturb_verts perturb_verts.cpp)
target_link_libraries(perturb_verts ${ALL_LIBS})

add_executable(walker walker.cpp)
target_link_libraries(walker ${ALL_LIBS})