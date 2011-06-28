set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS})

set(SRC fgv.cpp trackball.cpp)
FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

add_executable(fgv ${SRC} ${HEADERS} ${HEADER_INC})
target_link_libraries(fgv ${ALL_LIBS}) 

install(TARGETS fgv DESTINATION fgv)
install(DIRECTORY ${BASE_DIR}/scripts DESTINATION fgv)
 

