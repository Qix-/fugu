set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(hello_universe hello_universe.cpp)
target_link_libraries(hello_universe ${ALL_LIBS})

add_executable(hello_mesh hello_mesh.cpp)
target_link_libraries(hello_mesh ${ALL_LIBS})
	
add_executable(view_mesh view_mesh.cpp)
target_link_libraries(view_mesh ${ALL_LIBS})
	
