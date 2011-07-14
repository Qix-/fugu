#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/math/perlin_noise.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>

#include <vcg/complex/algorithms/smooth.h>

#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/extended_marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>

#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/complex/algorithms/refine.h>

// for opengl
#include <wrap/io_trimesh/export_obj.h>

using namespace std;
using namespace vcg;

typedef float ScalarType;

class MyVertex;
class MyFace;

struct MyUsedTypes : public UsedTypes<
Use<MyVertex>	::AsVertexType,
Use<MyFace>		::AsFaceType>{};

class MyVertex: public Vertex< MyUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::BitFlags, vertex::VFAdj, vertex::InfoOcf, vertex::Mark>{};
class MyFace: public Face< MyUsedTypes, face::FFAdj, face::Mark, face::VertexRef, face::Normal3f, face::BitFlags, face::InfoOcf> {};
class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

typedef SimpleVolume<SimpleVoxel> MyVolume;

void error(const char* msg){
	std::cerr << msg;
	exit(-1);
}

void error(const char* msg, const char* args){
	fprintf(stderr, msg, args);
	exit(-1);
}

double noise(double x, double y, double z){
	return math::Perlin::Noise(x,y,z);
}

extern "C" int init(lua_State* L){
	using namespace luabind;
	open(L);
	module(L)[
		def("noise", &noise)
	];
	return 0;
}

double callF(lua_State* L, double x, double y, double z, double t){
	lua_getglobal(L, "f");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);
	lua_pushnumber(L, t);
	if (lua_pcall(L, 4, 1, 0)!=0){
		error("Error running function \"f\": %s",lua_tostring(L, -1));
	}
	// else
	if (!lua_isnumber(L,-1)){
		error("function \"f\" didn't return a value!");
	}
	// else
	double result = lua_tonumber(L,-1);
	lua_pop(L, 1); // pop z, NB: function and args have been removed by pcall
	return result;
}

void callG(lua_State* L, double p[], double t, double dp[]){
	lua_getglobal(L, "g");
	lua_pushnumber(L, p[0]);
	lua_pushnumber(L, p[1]);
	lua_pushnumber(L, p[2]);
	lua_pushnumber(L, t);
	if (lua_pcall(L, 4, 1, 0)!=0){
		error("Error running function \"g\": %s",lua_tostring(L, -1));
	}
	// else
	if (!lua_istable(L,-1)){
		error("function \"g\" didn't return an array!");
	}
	// else
	for(int i=0;i<3;i++){
		lua_pushnumber(L, i+1);
		lua_gettable(L, -2);  /* get array[i+1] */
		if (!lua_isnumber(L, -1)){
			std::ostringstream oss;
			oss << "array doesnt contain a number!";
			error(oss.str().c_str());
		}
		dp[i] = lua_tonumber(L, -1);
		lua_pop(L,1);
	}
	lua_pop(L,1); // pop table
}

const char* scriptFileName = NULL;
lua_State *L;
bool loadingScript = false;

int main(int argc, char *argv[])
{
	if (argc!=2){
		std::cerr << "Usage: " << argv[0] << " <script.lua>\n";
		exit(0);
	}
	// else
	scriptFileName = argv[1];

	L = lua_open();   /* opens Lua */
	luaL_openlibs(L);             /* opens the libraries */
	init(L);

	if (luaL_dofile (L, scriptFileName)){
		error(lua_tostring(L, -1));
	}

	MyVolume	volume;

	typedef vcg::tri::TrivialWalker<MyMesh,MyVolume> MyWalker;
	typedef vcg::tri::MarchingCubes<MyMesh,MyWalker> MyMarchingCubes;
	MyWalker walker;

	// Simple initialization of the volume with some cool perlin noise
	int VOXELSIZE = 40;

	volume.Init(Point3i(VOXELSIZE,VOXELSIZE,VOXELSIZE));

	for(int i=0;i<VOXELSIZE;i++)
		for(int j=0;j<VOXELSIZE;j++)
			for(int k=0;k<VOXELSIZE;k++)
			{
				// volume.Val(i,j,k)=(j-32)*(j-32)+(k-32)*(k-32)  + i*10*(float)math::Perlin::Noise(i*.2,j*.2,k*.2);
				// volume.Val(i,j,k)=(j-VOXELSIZE/2)*(j-VOXELSIZE/2)+(k-VOXELSIZE/2)*(k-VOXELSIZE/2) + i*10*(float)math::Perlin::Noise(i*.1,j*.4,k*.4);
				// volume.Val(i,j,k) = (i>VOXELSIZE/2)?-1:1;
				double di = 2.0*i/VOXELSIZE - 1;
				double dj = 2.0*j/VOXELSIZE - 1;
				double dk = 2.0*k/VOXELSIZE - 1;
				volume.Val(i,j,k) = di*di + dj*dj + dk*dk - .5;
			}

	// MARCHING CUBES
	MyMesh mc_mesh;
	MyMarchingCubes	mc(mc_mesh, walker);
	walker.BuildMesh<MyMarchingCubes>(mc_mesh, volume, mc, 0); //20*20);

	// update bounding box
	vcg::tri::UpdateBounding<MyMesh>::Box(mc_mesh);
	// update Normals

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);

	for(int t=0;t<1000;t++){
		std::cout << t << "/1000\n";
		double time = t*0.01;


		// update
		for(int i=0;i<VOXELSIZE;i++){
			double di = 2.0*i/VOXELSIZE - 1;
			for(int j=0;j<VOXELSIZE;j++){
				double dj = 2.0*j/VOXELSIZE - 1;
				for(int k=0;k<VOXELSIZE;k++){
					double dk = 2.0*k/VOXELSIZE - 1;

					volume.Val(i,j,k) = (float) callF(L, di,dj,dk,time);
				}
			}
		}
		MyMarchingCubes	mc(mc_mesh, walker);
		walker.BuildMesh<MyMarchingCubes>(mc_mesh, volume, mc, 0);

		vcg::tri::UpdateTopology<MyMesh>::FaceFace(mc_mesh);
		vcg::tri::UpdateTopology<MyMesh>::VertexFace(mc_mesh);

		Refine(mc_mesh,MidPoint<MyMesh>(&mc_mesh));
		tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mc_mesh,2);

		// Refine(mc_mesh,MidPoint<MyMesh>(&mc_mesh));
		// Refine(mc_mesh,MidPointButterfly<MyMesh>());

		MyMesh::VertexIterator it = mc_mesh.vert.begin();
		for (;it!=mc_mesh.vert.end();++it){
			double dp[3];
			double p[3] = {it->P()[0],it->P()[1],it->P()[2]};
			callG(L,p,time,dp);
			it->P()[0] += dp[0];
			it->P()[1] += dp[1];
			it->P()[2] += dp[2];
		}

		// Refine(mc_mesh,MidPointButterfly<MyMesh>());
		tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mc_mesh,2);

		vcg::tri::UpdateNormals<MyMesh>::PerFace(mc_mesh);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mc_mesh);

		std::ostringstream oss;
		oss << "out" << std::setfill('0') << std::setw(4) << t << ".obj";
		vcg::tri::io::ExporterOBJ<MyMesh>::Save(mc_mesh,oss.str().c_str(),
				vcg::tri::io::Mask::IOM_VERTNORMAL
		);
	}

	lua_close(L);
	exit(EXIT_SUCCESS);
};
