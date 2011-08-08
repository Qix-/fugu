set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

add_executable(hello_spline hello_spline.cpp trackball.cpp)
target_link_libraries(hello_spline ${ALL_LIBS})

add_executable(hello_carrier hello_carrier.cpp trackball.cpp)
target_link_libraries(hello_carrier ${ALL_LIBS})

add_executable(hello_generalised_cylinder hello_generalised_cylinder.cpp trackball.cpp)
target_link_libraries(hello_generalised_cylinder ${ALL_LIBS})

add_executable(hello_ori hello_ori.cpp trackball.cpp)
target_link_libraries(hello_ori ${ALL_LIBS})

add_executable(hello_turtle hello_turtle.cpp trackball.cpp)
target_link_libraries(hello_turtle ${ALL_LIBS})

add_executable(hello_recsubdiv hello_recsubdiv.cpp trackball.cpp)
target_link_libraries(hello_recsubdiv ${ALL_LIBS})

add_executable(hello_morph hello_morph.cpp trackball.cpp)
target_link_libraries(hello_morph ${ALL_LIBS})

add_executable(hello_triangulate hello_triangulate.cpp trackball.cpp)
target_link_libraries(hello_triangulate ${ALL_LIBS})
