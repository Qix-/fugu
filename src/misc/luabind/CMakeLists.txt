add_executable(luabindstl luabindstl.cpp)
target_link_libraries(luabindstl ${LUA_LIBS} ${BOOST_LIBS})
