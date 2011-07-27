set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS}
	${BOOST_LIBS})

add_executable(hello_skin2 hello_skin2.cpp trackball.cpp)
target_link_libraries(hello_skin2 ${ALL_LIBS})

add_executable(hello_skin hello_skin.cpp trackball.cpp)
target_link_libraries(hello_skin ${ALL_LIBS})

add_executable(hello_armature hello_armature.cpp trackball.cpp)
target_link_libraries(hello_armature ${ALL_LIBS})

#add_executable(armature armature.cpp trackball.cpp)
#target_link_libraries(armature ${ALL_LIBS})

#add_executable(bones bones.cpp trackball.cpp)
#target_link_libraries(bones ${ALL_LIBS})

#add_executable(joints joints.cpp trackball.cpp)
#target_link_libraries(joints ${ALL_LIBS})

#add_executable(graph_test graph_test.cpp)
#target_link_libraries(graph_test ${ALL_LIBS})

#add_executable(graph_test2 graph_test2.cpp)
#target_link_libraries(graph_test2 ${ALL_LIBS})

#add_executable(nodetransform_test nodetransform_test.cpp trackball.cpp)
#target_link_libraries(nodetransform_test ${ALL_LIBS})

#add_executable(luabindstl luabindstl.cpp)
#target_link_libraries(luabindstl ${LUA_LIBS} ${BOOST_LIBS})

#add_executable(lb_refcounted lb_refcounted.cpp)
#target_link_libraries(lb_refcounted ${LUA_LIBS} ${BOOST_LIBS})