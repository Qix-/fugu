add_executable(luabindstl luabindstl.cpp)
target_link_libraries(luabindstl ${LUA_LIBS} ${BOOST_LIBS})

add_executable(lb_refcounted lb_refcounted.cpp)
target_link_libraries(lb_refcounted ${LUA_LIBS} ${BOOST_LIBS})