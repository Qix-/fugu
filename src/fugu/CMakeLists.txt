find_package(Qt4 COMPONENTS Qt3Support QtOpenGL QtGui QtCore REQUIRED)
set(QT_USE_QTOPENGL TRUE)
#set(QT_USE_IMPORTED_TARGETS TRUE)
SET( QT_USE_QT3SUPPORT TRUE )  
include(${QT_USE_FILE})

set(ALL_LIBS fg 
	${LUA_LIBS}
	${GL_LIBS} 
	${BOOST_LIBS}
	qscintilla2
	${QT_LIBRARIES}
	#${QT_QTOPENGL_LIBRARIES}
)

set(SRC main.cpp 
	trackball.cpp 
	shader.cpp
	#highlighter.cpp
	mainwindow.cpp
	fgview.cpp
	fglexer.cpp
	consolewidget.cpp
	#redirect.cpp
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
  #redirect.h
  qredirector.h
)

IF(UNIX)
	SET(MOC_HDR ${MOC_HDR} qredirectorimpl_posix.h)
ENDIF(UNIX)

QT4_WRAP_CPP(MOC_SRC ${MOC_HDR})

FILE(GLOB HEADERS "*.h")
FILE(GLOB HEADER_INC "*.inc")

SET(EVERYTHING ${SRC} ${MOC_SRC} ${HEADERS} ${HEADER_INC} ${UIS_H} ${RC_SRC})

# specify bundle properties
IF (FALSE)
IF (APPLE)
	SET(MACOSX_BUNDLE_ICON_FILE fugu_bundle.icns)
	SET_SOURCE_FILES_PROPERTIES(${BASE_DIR}/fugu_bundle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	SET(EVERYTHING ${EVERYTHING} ${BASE_DIR}/fugu_bundle.icns)
ENDIF()
ENDIF(FALSE)

IF(MINGW)
	add_executable(fugu ${EVERYTHING} ui/icon.rc) #add WIN32 to remove cmd window 
ELSE() #APPLE
	add_executable(fugu ${EVERYTHING}) # MACOSX_BUNDLE 
ENDIF() 
target_link_libraries(fugu ${ALL_LIBS}) 

# install rules

install(TARGETS fugu
	RUNTIME DESTINATION .
	BUNDLE DESTINATION .
	LIBRARY DESTINATION .
  	ARCHIVE DESTINATION .)


# install(DIRECTORY ui/res DESTINATION ui)
install(DIRECTORY ${BASE_DIR}/assets DESTINATION .)
install(DIRECTORY ${BASE_DIR}/scripts/ex DESTINATION scripts)
install(DIRECTORY ${BASE_DIR}/tools DESTINATION .)
install(DIRECTORY ${BASE_DIR}/core DESTINATION .)
install(FILES 
	${BASE_DIR}/README.txt
	${BASE_DIR}/LICENSE.txt
	${BASE_DIR}/CREDITS.txt
	DESTINATION .)

if (FALSE)
if (APPLE)
	set(APPS ${CMAKE_INSTALL_PREFIX}/fugu.app)  # paths to executables
	set(DIRS ${QT_LIBRARY_DIR})   # directories to search for prerequisites
	INSTALL(CODE "
   		include(BundleUtilities)
   		fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
   		" COMPONENT Runtime)
endif()
endif(FALSE)

if (APPLE)
	# INSTALL QT LIBRARIES
	INSTALL(FILES
	${QT_LIBRARY_DIR}/Qt3Support.framework/Versions/4/Qt3Support
	${QT_LIBRARY_DIR}/QtSql.framework/Versions/4/QtSql
	${QT_LIBRARY_DIR}/QtCore.framework/Versions/4/QtCore
	${QT_LIBRARY_DIR}/QtOpenGL.framework/Versions/4/QtOpenGL
	${QT_LIBRARY_DIR}/QtXml.framework/Versions/4/QtXml
	${QT_LIBRARY_DIR}/QtGui.framework/Versions/4/QtGui
	${QT_LIBRARY_DIR}/QtNetwork.framework/Versions/4/QtNetwork
	DESTINATION .
	)

	INSTALL(DIRECTORY
	${BASE_DIR}/qt_menu.nib
	DESTINATION .)

	SET(CPACK_GENERATOR "Bundle")		
	SET(CPACK_PACKAGE_FILE_NAME "FuguOSX.${FG_VERSION_MAJOR}.${FG_VERSION_MINOR}.${FG_VERSION_PATCH}")
	SET(CPACK_BUNDLE_ICON "${BASE_DIR}/fugu_bundle.icns")
	SET(CPACK_BUNDLE_NAME "FuguOSX")
	SET(CPACK_BUNDLE_PLIST "${BASE_DIR}/Info.plist")
	SET(CPACK_BUNDLE_STARTUP_COMMAND "${BASE_DIR}/src/run_fugu_osx.sh")
	INCLUDE(CPack)

endif()
