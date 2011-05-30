add_executable(hello_universe hello_universe.cpp)
target_link_libraries(hello_universe fg 
	${LUA_LIBS} 
	libboost_filesystem-mgw34-mt-1_40
	libboost_system-mgw34-mt-1_40)
