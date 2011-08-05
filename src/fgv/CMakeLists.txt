set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS}
	AntTweakBar)

set(SRC fgv.cpp trackball.cpp shader.cpp)
FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

add_executable(fgv ${SRC} ${HEADERS} ${HEADER_INC})
target_link_libraries(fgv ${ALL_LIBS}) 

# install fgv into bin
install(TARGETS fgv DESTINATION .)

if(WIN32)
	file(COPY "${BASE_DIR}/lib/msw/AntTweakBar.dll" DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(WIN32)

if(APPLE)
# still can't get rpath to work properly with cmake
	file(COPY "${BASE_DIR}/lib/osx/libAntTweakBar.dylib" DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(APPLE)

# don't install scripts, just use the fg/scripts directory as is 
# install(DIRECTORY ${BASE_DIR}/scripts DESTINATION fgv)
 

