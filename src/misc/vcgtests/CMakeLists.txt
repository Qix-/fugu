SET(ALL_LIBS ${LUA_LIBS} ${GL_LIBS})

add_executable(perturb_verts perturb_verts.cpp)
target_link_libraries(perturb_verts ${ALL_LIBS})

add_executable(splatter splatter.cpp)
target_link_libraries(splatter ${ALL_LIBS})

add_executable(splatandraise splatandraise.cpp)
target_link_libraries(splatandraise ${ALL_LIBS})

# add_executable(walker walker.cpp)
# target_link_libraries(walker ${ALL_LIBS})

add_executable(extrude extrude.cpp)
target_link_libraries(extrude ${ALL_LIBS})

add_executable(nringtest nringtest.cpp)
target_link_libraries(nringtest ${ALL_LIBS})