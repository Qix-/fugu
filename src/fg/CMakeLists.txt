set(SRC 
	armature.cpp
	bindings.cpp
	carriercurve.cpp
	crosssectioncircular.cpp	
	fg.cpp
	functions.cpp
	generalisedcylinder.cpp	
	glrenderer.cpp
	glrenderer_glutprimitives.cpp	
	interpcrosssec.cpp	
	lincarrier.cpp
	mat4.cpp	
	mesh.cpp	
	meshimpl.cpp
	meshoperators.cpp
	meshoperators_vcg.cpp
	node.cpp
	nodegraph.cpp	
	pbezcarrier.cpp
	pbezinterpdiv.cpp
	plylib.cpp	
	proxy.cpp
	quat.cpp	
	recsubdiv.cpp	
	scaleinterpcrosssec.cpp
	turtle.cpp	
	universe.cpp	
	vec3.cpp
	vertex.cpp	
	)

set(HDRS
	armature.h
	bezinterp.h
	bindings.h
	carriercurve.h
	lincarrier.h
	pbezcarrier.h
	crosssection.h
	crosssectioncircular.h
	fg.h
	functions.h
	generalisedcylinder.h
	glrenderer.h
	interpcrosssec.h
	interpolator.h
	lininterp.h
	mat4.h
	mesh.h
	meshimpl.h
	meshnode.h
	meshoperators.h
	meshoperators_vcg.h
	node.h
	nodegraph.h
	operator.h
	pbezinterp.h
	pbezinterpdiv.h
	proxy.h
	quat.h
	recsubdiv.h
	scaleinterpcrosssec.h
	turtle.h
	universe.h
	util.h
	vec3.h
	vertex.h)

set(INLS
	bezinterp.inl
	functions.inl
	interpolator.inl
	lininterp.inl
	pbezinterp.inl
)

# FILE(GLOB HEADERS "*.h")
# FILE(GLOB HEADER_INC "*.inc")

add_library(fg ${SRC} ${HDRS} ${INLS})
target_link_libraries(fg ${LUA_LIBS} ${GL_LIBS})
