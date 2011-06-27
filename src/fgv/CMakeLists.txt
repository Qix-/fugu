set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(fgv fgv.cpp trackball.cpp)
target_link_libraries(fgv ${ALL_LIBS}) 