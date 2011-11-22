find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL)
SET( QT_USE_QT3SUPPORT TRUE )  
include(${QT_USE_FILE})

set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS}
	qscintilla2
	${QT_LIBRARIES}
	${QT_QTOPENGL_LIBRARIES}
)

set(SRC main.cpp 
	trackball.cpp 
	shader.cpp
	#highlighter.cpp
	mainwindow.cpp
	fgview.cpp
	fglexer.cpp
	consolewidget.cpp
	redirect.cpp
	exporter.cpp
	html_template.cpp
	qredirector.cpp
	)

set(SRCS ${SRC})

set(UIS
	ui/mainwindow.ui
	ui/exportdialog.ui
)
QT4_WRAP_UI(UIS_H ${UIS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # required to be able to include the generated ui headers

SET(RCS
	ui/resources.qrc
)	
QT4_ADD_RESOURCES( RC_SRC ${RCS} )
  
INCLUDE_DIRECTORIES(
   ${QT_INCLUDE_DIR}
   ${QT_QTOPENGL_INCLUDE_DIR}
)

SET(MOC_HDR
  mainwindow.h
  #highlighter.h
  fgview.h
  fglexer.h
  consolewidget.h
  redirect.h
  qredirector.h
)

IF(UNIX)
	SET(MOC_HDR ${MOC_HDR} qredirectorimpl_posix.h)
ENDIF(UNIX)

QT4_WRAP_CPP(MOC_SRC ${MOC_HDR})

FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

SET(EVERYTHING ${SRC} ${MOC_SRC} ${HEADERS} ${HEADER_INC} ${UIS_H} ${RC_SRC})

IF(MINGW)
	add_executable(fugu ${EVERYTHING} ui/icon.rc) #add WIN32 to remove cmd window 
ELSE(MINGW)
	add_executable(fugu ${EVERYTHING})
ENDIF(MINGW)
target_link_libraries(fugu ${ALL_LIBS}) 

# install fgv into bin
install(TARGETS fugu DESTINATION .)

install(DIRECTORY ui/res DESTINATION ui/res)


