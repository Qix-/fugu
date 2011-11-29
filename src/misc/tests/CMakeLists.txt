set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(linear_algebra linear_algebra.cpp)
target_link_libraries(linear_algebra ${ALL_LIBS})
