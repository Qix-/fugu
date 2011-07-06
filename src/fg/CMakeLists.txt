set(SRC 
	fg.cpp
	universe.cpp
	bindings.cpp
	mesh.cpp
	vec3.cpp
	functions.cpp
	meshimpl.cpp
	meshoperators.cpp
	meshoperators_vcg.cpp
	proxy.cpp
	vertex.cpp
	glrenderer.cpp
	carriercurve.cpp
	carriercurvelinear.cpp
	carriercurvepiecewisebezier.cpp
	crosssectioncircular.cpp
	generalisedcylinder.cpp
	turtle.cpp
	node.cpp
	mat4.cpp
	plylib.cpp
	nodegraph.cpp)

set(HDRS
	bezierinterpolator.h
	bindings.h
	carriercurve.h
	carriercurvepiecewisebezier.h
	crosssection.h
	crosssectioncircular.h
	fg.h
	functions.h
	generalisedcylinder.h
	glrenderer.h
	interpolator.h
	mat4.h
	mesh.h
	meshimpl.h
	meshnode.h
	meshoperators.h
	meshoperators_vcg.h
	node.h
	nodegraph.h
	operator.h
	piecewisebezierinterpolator.h
	proxy.h
	universe.h
	vec3.h
	vertex.h)

set(INLS
	bezierinterpolator.inl
	functions.inl
	interpolator.inl
	piecewisebezierinterpolator.inl
)

# FILE(GLOB HEADERS "*.h")
# FILE(GLOB HEADER_INC "*.inc")

add_library(fg ${SRC} ${HDRS} ${INLS})
target_link_libraries(fg ${LUA_LIBS} ${GL_LIBS})
