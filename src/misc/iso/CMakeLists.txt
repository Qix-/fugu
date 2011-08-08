SET(ALL_LIBS ${LUA_LIBS} ${GL_LIBS})
add_executable(iso iso.cpp)
target_link_libraries(iso ${ALL_LIBS})

add_executable(iso_off iso_off.cpp ${BASE_DIR}/include/vcg/wrap/ply/plylib.cpp)
target_link_libraries(iso_off ${ALL_LIBS})

add_executable(iso_smooth iso_smooth.cpp)
target_link_libraries(iso_smooth ${ALL_LIBS})
