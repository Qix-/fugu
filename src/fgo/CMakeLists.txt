set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

set(SRC fgo.cpp)
FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

add_executable(fgo ${SRC} ${HEADERS} ${HEADER_INC})
target_link_libraries(fgo ${ALL_LIBS})
 
install(TARGETS fgo DESTINATION .)



