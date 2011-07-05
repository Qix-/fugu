set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(linear_algebra linear_algebra.cpp)
target_link_libraries(linear_algebra ${ALL_LIBS})

add_test(linear_algebra ${CMAKE_CURRENT_BINARY_DIR}/linear_algebra)