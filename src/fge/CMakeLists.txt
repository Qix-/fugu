find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL)
include(${QT_USE_FILE})

set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS}
	qscintilla2
	AntTweakBar)

set(SRC main.cpp 
	trackball.cpp 
	shader.cpp
	highlighter.cpp
	mainwindow.cpp
	fgview.cpp
	fglexer.cpp
	consolewidget.cpp
	)

set(SRCS ${SRC})

set(UIS

)
QT4_WRAP_UI(UIS_H ${UIS})

INCLUDE_DIRECTORIES(
   ${QT_INCLUDE_DIR}
   ${QT_QTOPENGL_INCLUDE_DIR}
)

set(ALL_LIBS ${ALL_LIBS}
   ${QT_LIBRARIES}
   ${QT_QTOPENGL_LIBRARIES}
)

SET(MOC_HDR
  mainwindow.h
  highlighter.h
  fgview.h
  consolewidget.h
)

QT4_WRAP_CPP(MOC_SRC ${MOC_HDR})

FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

add_executable(fugu ${SRC} ${MOC_SRC} ${HEADERS} ${HEADER_INC})
target_link_libraries(fugu ${ALL_LIBS}) 

# install fgv into bin
install(TARGETS fugu DESTINATION .)

if(WIN32)
	file(COPY "${BASE_DIR}/lib/msw/AntTweakBar.dll" DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(WIN32)

if(APPLE)
# still can't get rpath to work properly with cmake
# so use DYLD_LIBRARY_PATH
endif(APPLE)

# don't install scripts, just use the fg/scripts directory as is 
# install(DIRECTORY ${BASE_DIR}/scripts DESTINATION fgv)
 

