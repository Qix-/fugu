set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(hello_carrier hello_carrier.cpp trackball.cpp)
add_executable(hello_generalised_cylinder hello_generalised_cylinder.cpp trackball.cpp)
target_link_libraries(hello_carrier ${ALL_LIBS})
target_link_libraries(hello_generalised_cylinder ${ALL_LIBS})
