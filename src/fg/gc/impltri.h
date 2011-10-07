#ifndef FG_GC_IMPL_TRI_H
#define FG_GC_IMPL_TRI_H

#include <vector>
#include <list>
#include <utility>

#include "fg/gc/generalisedcylinderimpl.h"
#include "fg/gc/frontpoly.h"

namespace fg {
	namespace gc {

class ImplTri {
	public:
	  ImplTri( const std::vector<const GeneralisedCylinderImpl*> &data );
	  ImplTri( const GeneralisedCylinderImpl &data );

	  void triangulate( double delta, std::vector<Vec3> &points, std::vector<int> &triangles ); 
	  void surfaceNearPoint( const Vec3 &q, Vec3 &p, Vec3 &n, Vec3 &t1, Vec3 &t2, double &u, double &v, double &f );
	  std::list<int> buildFrontPoly( double delta, int cyl, double u, double v, double dir = -1. );
	  void updateFrontAngles( std::list<int>::iterator &FPi );
	  double polarAngle( double x, double y );
	  void calcBV( int i );
	  std::list<int>::iterator minFA( );
	  void getNeighbors( std::list<int>::iterator &p, std::list<int>::iterator &v1, std::list<int>::iterator &v2 );
	  Vec3 project( int p, int v );

	private:
	  std::vector<const GeneralisedCylinderImpl*> mData;
	  std::vector< std::list<int> > mFPolys;
	  std::vector< std::pair< Vec3, Vec3 > > mBV;
	  std::vector<P> mPoints;
	  static const int mMaxIts = 100;
	  static const double mThreshHold = 1E-30;

      void getInterDistNeigh( std::list<int>::iterator &v, std::list<int> &neigh );
	  bool interDistCheck( double delta );
	  bool extDistCheck( double delta, std::vector<int> &triangles );
	  void addTri( std::list<int>::iterator &mA, double delta, std::vector<int> &triangles );
};

	}
}
#endif

