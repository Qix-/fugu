
#include <iostream>
#include <algorithm>

#include "fg/mat4.h"
#include "fg/gc/impltri.h"


namespace fg {
	namespace gc {

ImplTri::ImplTri( const std::vector<const GeneralisedCylinderImpl*> &data )
{
	for( int i = 0; i < data.size(); ++i ) {
		mData.push_back( data[i] );
	}
}

ImplTri::ImplTri( const GeneralisedCylinderImpl &data )
{
	mData.push_back( &data );
}

void ImplTri::triangulate( double delta, std::vector<Vec3> &points, std::vector<int> &triangles )
{
	points.clear();
	triangles.clear();

	mPoints.clear();
	mFPolys.clear();

	// Build front polys
	buildFrontPoly( delta, 0, 1., 0., -1. );
	buildFrontPoly( delta, 0, 1., 1., 1. );

	// Calc Bounding volumes
	mBV.push_back( std::pair<Vec3, Vec3>( Vec3(0.,0.,0.), Vec3(0., 0., 0.)) );
	mBV.push_back( std::pair<Vec3, Vec3>( Vec3(0.,0.,0.), Vec3(0., 0., 0.)) );
	calcBV( 0 );
	calcBV( 1 );

	// Init the front angles on the actuall front poly
	for( std::list<int>::iterator i = mFPolys.front().begin(); i != mFPolys.front().end(); ++i )
	{
		updateFrontAngles( i );
	}

	bool cont = true;
	int n = 0;

	loop:
	while( cont ) {
		if( mFPolys.front().size() == 3 )
		{
			mFPolys.erase( mFPolys.begin() );
			if( mFPolys.size() == 0 )
				break;

			goto loop;
		}
//		if( n > 400 )
//			cont = false;
		// Find the min front angle
		// Check if we should finish
		n++;
		std::list<int>::iterator mA = minFA();
		//std::cout << "mA = " << mPoints[*mA].mFrontAngle << "\n";
	
		// Check distance
		if( mPoints[*mA].mFrontAngle > M_PI / 3. )
		{
			//std::cout << "n = " << n << "\n";
			if( !interDistCheck( delta ) )
			{
				//std::cout << "Done an inner dist. n = " << n << "\n\n";
				goto loop;
			}
			if( !extDistCheck( delta, triangles ) )
			{
			//	// Get all the points into output
			//	points.resize( mPoints.size() );
			//	for( int i = 0; i< mPoints.size(); ++i )
			//	{
			//		points[i] = mPoints[i].mPos;
			//	}
			//	return;
				goto loop;
			}
		}
		{

			addTri( mA, delta, triangles );
			//std::cout << "num points = " << mPoints.size() << std::endl;
			/*
			// How many triangles to add
			std::list<int>::iterator v1, v2;
			getNeighbors( mA, v1, v2 );
			//std::cout << "v1 = " << *v1 << ", mA = " << *mA << ", v2 = " << *v2 << "\n";
			int nt = (int) floor(3. * mPoints[*mA].mFrontAngle / M_PI) + 1;

			double delw = mPoints[*mA].mFrontAngle / nt;

			//std::cout << "w = " << mPoints[*mA].mFrontAngle << ", nt = " << nt << ", delw = " << delw << "\n"; 
	
			// Too small
			if( delw < 0.8 && nt > 1 )
			{
				--nt;
				delw = mPoints[*mA].mFrontAngle / nt;
			}
			// too big
			else if( nt == 1 && delw > .8 && 
				(mPoints[*v1].mPos - mPoints[*v2].mPos).length() > 1.2*delta)
			{
				nt = 2;
				delw *= .5;
			}
			// Too big
			else if( mPoints[*mA].mFrontAngle < 3. &&
				     (( mPoints[*v1].mPos - mPoints[*mA].mPos).length() < .5 * delta ||
					  ( mPoints[*v2].mPos - mPoints[*mA].mPos).length() < .5 * delta ))
			{
				nt = 1;
			}
	
			// Add new triangles
			std::list<int> newPs;
			newPs.clear();
			if( nt == 1 )
			{
				std::cout << "size for one t = " << mPoints.size();
				triangles.push_back( *v1 );
				triangles.push_back( *v2 );
				triangles.push_back( *mA );
			}
			else
			{
				double u, v;
				P &p = mPoints[*mA];
				Vec3 p_i, n, t1, t2;
				Vec3 q_i = project( *mA, *v1 ); 
				//std::cout << "nt = " << nt << ", delw = " << delw << ", w = " << mPoints[*mA].mFrontAngle << "\n";
	
				Mat4 m;
				
				for( int i = 0; i < nt - 1; ++i )
				{
					// Perform rotation
					// Tralslate mA to origin
					q_i = q_i - p.mPos;
					// Rotate around the norm
					m = m.setRotateRad( delw, p.mNorm ); 
					q_i = m * q_i;
					// Translate back to correct pos
					q_i = q_i + p.mPos;
	
					surfaceNearPoint( q_i, p_i, n, t1, t2, u, v );
					if( mPoints.size() - 1 == 44)//( p_i - Vec3(-0.156239,0.987719,0.273178)).length() < 0.001)
					{
						std::cout << q_i << "->" << p_i << "\n";
						std::cout << "u, v = " << u << ", " << v << "\n";
					}
				    //std::cout << "p_" << i+1 << " = " << p_i << "\n";
					mPoints.push_back( P( p_i, n, t1, t2 ) );
					newPs.push_back( mPoints.size() - 1 );
				}

				// Add the triangles
				// The first
				//std::cout << "ma = " << *mA << << std::endl;
				std::list<int>::iterator i = newPs.begin();
				std::list<int>::iterator i1 = newPs.begin();
				++i1;

				triangles.push_back( *v1 );
				triangles.push_back( *i );
				triangles.push_back( *mA );
				// The middle
				while( i1 != newPs.end() ) {
					triangles.push_back( *i );
					triangles.push_back( *i1 );
					triangles.push_back( *mA );
					++i;
					++i1;
				}
				// The last
				triangles.push_back( *i );
				triangles.push_back( *v2 );
				triangles.push_back( *mA );
			}
	
	
			// update the front
			// delete mA
			mFPolys.front().erase( mA );
			// Add the new points
			mFPolys.front().splice( v2, newPs );
			// Recalc the required front angles, just do all for now but only should do those that need it!
			for( std::list<int>::iterator i = mFPolys.front().begin(); i != mFPolys.front().end(); ++i )
			{
				updateFrontAngles( i );
			}
			// recalculate the bounding volume
			calcBV( 0 );
			*/

		}
	}

	// Get all the points into output
	points.resize( mPoints.size() );
	for( int i = 0; i< mPoints.size(); ++i )
	{
		points[i] = mPoints[i].mPos;
	}
}

Vec3 ImplTri::project( int p, int v )
{
	P &o = mPoints[p];
	Vec3 &vt = mPoints[v].mPos;

	double distance = o.mNorm.dot( vt - o.mPos );
	return vt - o.mNorm * distance;
	/*
	P &o = mPoints[p];
    Vec3 vt = mPoints[v].mPos - o.mPos;
	vt = o.mNorm * ( vt.dot( o.mNorm ) / o.mNorm.lengthSquared() ) + o.mPos;

	return vt;
	*/
}

void ImplTri::getNeighbors( std::list<int>::iterator &p, std::list<int>::iterator &v1, std::list<int>::iterator &v2 )
{
//	std::cout << "Before p = " << *p << std::endl;
//	std::cout << "front of list = " << *(mFPolys.front().begin()) << std::endl;
	// One point behind
	if( *p == *(mFPolys.front().begin()) )
	{
		v1 = mFPolys.front().end();
		--v1;
	}
	else
	{
		v1 = std::list<int>::iterator( p );
		--v1;
	}
	//std::cout << *p << std::endl;
	
	// One point ahead
	if( *p == mFPolys.front().back() )
	{
		v2 = mFPolys.front().begin();
	}
	else
	{
		v2 = std::list<int>::iterator( p );
		++v2;
	}
//	std::cout << "after p = " << *p << std::endl;
//	std::cout << "v1 = " << *v1 << std::endl;
//	std::cout << "v2 = " << *v2 << std::endl << std::endl;
}

void ImplTri::updateFrontAngles( std::list<int>::iterator &FPi )
{
	Vec3 v1, v2;
	std::list<int>::iterator v1i, v2i;

	getNeighbors( FPi, v1i, v2i );

	P &p = mPoints[*FPi];
	v1 = mPoints[*v1i].mPos;
	v2 = mPoints[*v2i].mPos;

	// Transform into the local coords of the point in question
	double m[16];

	m[0] = p.mT1.getX();  
	m[1] = p.mT1.getY();  
	m[2] = p.mT1.getZ();  
	m[3] = 1.;

	m[4] = p.mT2.getX();
	m[5] = p.mT2.getY();
	m[6] = p.mT2.getZ();
	m[7] = 1.;

	m[8] =  p.mNorm.getX();
	m[9] =  p.mNorm.getY();
	m[10] = p.mNorm.getZ();
	m[11] = 1.;

	m[12] = 0.;
	m[13] = 0.;
	m[14] = 0.;
	m[15] = 1.;

	double x1,y1,x2,y2;
	double w1, w2;
	v1 = v1 - p.mPos;
	x1 = m[0] * v1.getX() + m[1] * v1.getY() + m[2] * v1.getZ();
	y1 = m[4] * v1.getX() + m[5] * v1.getY() + m[6] * v1.getZ();
	w1 = atan2( y1, x1 );
	//w1 = polarAngle( x1, y1 );

	v2 = v2 - p.mPos;
	x2 = m[0] * v2.getX() + m[1] * v2.getY() + m[2] * v2.getZ();
	y2 = m[4] * v2.getX() + m[5] * v2.getY() + m[6] * v2.getZ();
	w2 = atan2( y2, x2 );
	//w2 = polarAngle( x2, y2 );

//	std::cout << "v1 = " << v1 + p.mPos << ", p = " << p.mPos << ", v2 = " << v2 + p.mPos << "\n";
//	std::cout << "t1 = " << p.mT1 << ", t2 = " << p.mT2 << ", n = " << p.mNorm << "\n";
//	std::cout << "x1, y1 = " << x1 << ", " << y1 << "\n";
//	std::cout << "x2, y2 = " << x2 << ", " << y2 << "\n";
//	std::cout << "w1 = " << w1 << ", w2 = " << w2 << std::endl;

	if( w2 > w1 )
		p.mFrontAngle = w2 - w1;
	else
		p.mFrontAngle = w2 - w1 + 2.*M_PI;
//	std::cout << "w = " << p.mFrontAngle << std::endl;
}

std::list<int> ImplTri::buildFrontPoly( double delta, int cyl, double u, double v, double dir )
{
	mFPolys.push_back( std::list<int>() );
	std::list<int> &fp = mFPolys.back();

	bool cont = true;
	Vec3 pos, t1, t2, n, g;
	double d, f;
	P point;

	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v, f );

	point = P( pos, n, t1, t2, u, v, f );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	while( cont ) {
		u += dir*delta / mData[0]->getDerU( u, v ).length();
		g = mData[0]->getPosition( u, v );
		surfaceNearPoint( g, pos, n, t1, t2, u, v, f );
		point = P( pos, n, t1, t2, u, v, f );
		mPoints.push_back( point );
		fp.push_back( mPoints.size() - 1 );

		if( (fp.size() > 2 && (mPoints[fp.front()].mPos - mPoints[fp.back()].mPos).length() < 1.5*delta) || u < 0. )
		  cont = false;
	}
/*
	Vec3 pos, t1, t2, n, g;
	double d;
	P point;

	u = .8;
	v = 0.;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = 0.;
	v = 0.;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .2;
	v = 0.;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .25;
	v = 0.8;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .2;
	v = 1.6;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .0;
	v = .6;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .8;
	v = 1.6;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	u = .8;
	v = .8;
	g = mData[cyl]->getPosition( u, v );
	surfaceNearPoint( g, pos, n, t1, t2, u, v );

	point = P( pos, n, t1, t2 );
	mPoints.push_back( point );
	fp.push_back( mPoints.size() - 1 );

	reverse( fp.begin(), fp.end() );
	*/
    /*
	*/

	return fp;
}

void ImplTri::surfaceNearPoint( const Vec3 &q, Vec3 &p, Vec3 &n, Vec3 &t1, Vec3 &t2, double &u, double &v, double &f )
{
	Vec3 u_i = q;
	Vec3 du;
	Vec3 fgrad = Vec3(0., 0., 0.);

	for( int i = 0; i < mMaxIts; ++i )
	{
		/********
		 * CHANGE THIS!
		 *******/
		if( mPoints.size() - 1 == 44 )// (q - Vec3(0.156239,-0.987719,0.273178)).length() < 0.01 )
		{
			//std::cout << "Calling impl( " << u_i << ", " << u << ", " << v << ", " << f << ", " << fgrad << ")\n";
//			std::cout << q << "\n";
//			std::cout << fgrad << "\n";
//			std::cout << "u, v = " << u << ", " << v << "\n";
		}

		mData[0]->impl( u_i, u, v, f, fgrad );
		du = fgrad * f /( fgrad.lengthSquared() );

		u_i = u_i - du;

		if( du.lengthSquared() < mThreshHold )
			break;
	}

	p = u_i;

	n = fgrad;

	if( n.getX() > .5 || n.getY() > .5 )
	{
		t1 = Vec3( n.getY(), -n.getX(), 0. );
		t1.normalise();
	} else
	{
		t1 = Vec3( -n.getZ(), 0., n.getX() );
		t1.normalise();
	}

	t2 = n.cross(t1);
}

double ImplTri::polarAngle( double x, double y )
{
	if( fabs( x ) < 1E-12 )
	{
		if( fabs( y ) < 1E-12 )
			return 0.;
		else if( y < 0. )
			return -.5 * M_PI;
		else
			return .5 * M_PI;
	}
	else if( x < 0. )
	{
		if( y < 0)
			atan( y / x ) - M_PI;
		else
			atan( y / x ) + M_PI;
	} else 
		return atan( y / x );
}

void ImplTri::calcBV( int i )
{
	Vec3 &min = mBV[i].first;
	Vec3 &max = mBV[i].second;

	if( i >= mFPolys.size() || mFPolys[i].size() == 0 )
		return;

	std::list<int>::iterator it = mFPolys[i].begin();

	max = mPoints[*it].mPos;
	min = mPoints[*it].mPos;

	++it;

	for( ; it != mFPolys[i].end(); ++it )
	{
		Vec3 pos = mPoints[*it].mPos;

		if( pos.getX() < min.getX() )
			min.setX( pos.getX() );
		else if( pos.getX() > max.getX() )
			max.setX( pos.getX() );

		if( pos.getY() < min.getY() )
			min.setY( pos.getY() );
		else if( pos.getY() > max.getY() )
			max.setY( pos.getY() );

		if( pos.getZ() < min.getZ() )
			min.setZ( pos.getZ() );
		else if( pos.getZ() > max.getZ() )
			max.setZ( pos.getZ() );
	}
}

std::list<int>::iterator ImplTri::minFA( )
{
	std::list<int>::iterator it = mFPolys[0].begin();

	std::list<int>::iterator mi = it;
	double ma = mPoints[*it].mFrontAngle;
	++it;

	for( ; it != mFPolys[0].end(); ++it )
	{
		if( mPoints[*it].mFrontAngle < ma )
		{
			mi = it;
			ma = mPoints[*it].mFrontAngle;
		}
	}

	return mi;
}

bool ImplTri::extDistCheck( double delta, std::vector<int> &triangles )
{
	for( int i = 1; i < mFPolys.size(); ++i )
	{
		// Test the bounding volumes
		if( ((mBV[0].first.getX() - 2.*delta < mBV[i].first.getX() && mBV[i].first.getX() < mBV[0].second.getX() + 2.*delta)
			 || (mBV[0].first.getX() - 2.*delta < mBV[i].second.getX() && mBV[i].second.getX() < mBV[0].second.getX() + 2.*delta))
		  &&((mBV[0].first.getY() - 2.*delta < mBV[i].first.getY() && mBV[i].first.getY() < mBV[0].second.getY() + 2.*delta)
			 || (mBV[0].first.getY() - 2.*delta < mBV[i].second.getY() && mBV[i].second.getY() < mBV[0].second.getY() + 2.*delta))
		  &&((mBV[0].first.getZ() - 2.*delta < mBV[i].first.getZ() && mBV[i].first.getZ() < mBV[0].second.getZ() + 2.*delta)
			 || (mBV[0].first.getZ() - 2.*delta < mBV[i].second.getZ() && mBV[i].second.getZ() < mBV[0].second.getZ() + 2.*delta)))
		{
			std::list<int>::iterator u, v;

			// Keep track of smallest so far
			double smallest = 2.*delta;
			std::list<int>::iterator us, vs;

			// Test actual points
			u = mFPolys.front().begin();
			v = mFPolys[i].begin();
			for( ; u != mFPolys.front().end(); ++u )
			{
				for( ; v != mFPolys[i].end(); ++v )
				{
					//std::cout << "dist " << *u << " and " << *v << " = " << (mPoints[*u].mPos - mPoints[*v].mPos).length() << ", delta = " << delta << "\n";
					if( ((mPoints[*u].mPos - mPoints[*v].mPos).length() < 1.1*delta ) && mPoints[*u].mDistCheck && mPoints[*v].mDistCheck )
					{
						//std::cout << "Here\n";
						if( smallest > (mPoints[*u].mPos - mPoints[*v].mPos).length())
						{
							smallest = (mPoints[*u].mPos - mPoints[*v].mPos).length();
							us = u;
							vs = v;
						}
					}

				}
				if( smallest < 1.1*delta )
				{
		//			std::cout << "Real collision!\n";

					// The new front poly
					int ui = *us;
					int vi = *vs;
					std::list<int> tmp;
					tmp.insert( tmp.begin(), mFPolys[0].begin(), ++us );
					--u;
					tmp.insert( tmp.end(), vs, mFPolys[1].end() );
					tmp.insert( tmp.end(), mFPolys[1].begin(), ++vs );
					--v;
					tmp.insert( tmp.end(), us, mFPolys[0].end() );

					//std::cout << "fp[0]\n";
					//for( std::list<int>::iterator it = mFPolys.front().begin(); it != mFPolys.front().end(); ++it )
					//	std::cout << *it << std::endl;
					//std::cout << "fp[1]\n";
					//for( std::list<int>::iterator it = mFPolys[i].begin(); it != mFPolys[i].end(); ++it )
					//	std::cout << *it << std::endl;
					//std::cout << "new\n";
					//for( std::list<int>::iterator it = tmp.begin(); it != tmp.end(); ++it )
					//	std::cout << *it << std::endl;

					//std::cout << "Collision at = " << *u << ", " << *v << "\n";

					// delete the old one
					mFPolys.erase( mFPolys.begin() + i );
					mBV.erase( mBV.begin() + i );

					// Replace the actual front
					mFPolys[0] = tmp;

					// Get rid of doubles!
					std::list<int>::iterator it = find( mFPolys.front().begin(), mFPolys.front().end(), ui );
					//std::cout << "it = " << *it << "\n";
					//++it;
					//std::cout << "++it = " << *it << "\n";
					//--it;
					updateFrontAngles( it );
					//std::cout << "fa of it = " << mPoints[*it].mFrontAngle << "\n";
					addTri( it, delta, triangles );

					it = find( mFPolys.front().begin(), mFPolys.front().end(), vi );
					updateFrontAngles( it );
					addTri( it, delta, triangles );

					// update all the things!
					calcBV( 0 );
					for( std::list<int>::iterator it2 = mFPolys.front().begin(); it2 != mFPolys.front().end(); ++it2 )
					{
						updateFrontAngles( it2 );
					}

//					std::cout << "fp[0]\n";
//					for( std::list<int>::iterator it = mFPolys.front().begin(); it != mFPolys.front().end(); ++it )
//						std::cout << *it << std::endl;

					return false;

		//			std::cout << "Collision at = " << *u << ", " << *v << "\n";
					//exit(0);
					}
			}
		}

	}

	return true;
}

bool ImplTri::interDistCheck( double delta )
{
	std::list<int>::iterator v, u;
	std::list<int> neigh;

	for( v = mFPolys.front().begin(); v != mFPolys.front().end(); ++v )
	{
		getInterDistNeigh( v, neigh );

		for( u = neigh.begin(); u != neigh.end(); ++u )
		{
			//std::cout << "v = " << *v << "\n";
	
			//for( std::list<int>::iterator i = mFPolys.front().begin(); i != mFPolys.front().end(); ++i )
			//	std::cout << *i << std::endl;
	
			//std::cout << "\n";
			//for( std::list<int>::iterator i = neigh.begin(); i != neigh.end(); ++i )
			//	std::cout << *i << std::endl;
			//std::cout << "\n";
			//std::cout << "\n";

			//std::cout << (mPoints[*u].mPos - mPoints[*v].mPos).length() << "\n";

			if( mPoints[*u].mDistCheck && mPoints[*v].mDistCheck && (mPoints[*u].mPos - mPoints[*v].mPos).length() < delta )
			{
				// Split the actual front
				std::list<int> tmp1, tmp2;
				std::list<int>::iterator it;
				it = v;
				++it;
				// first part
				tmp1.insert( tmp1.begin(), mFPolys.front().begin(), it );
				it = find( mFPolys.front().begin(), mFPolys.front().end(), *u );
				tmp1.insert( tmp1.end(), it, mFPolys.front().end() );

				// second part
				++it;
				tmp2.insert( tmp2.begin(), v, it );

				// replace actual front
				mFPolys.front() = tmp1;
				// add the new front
				mFPolys.push_back( tmp2 );

				mPoints[*u].mDistCheck = false;
				mPoints[*v].mDistCheck = false;

				// recalc the BV's
				calcBV(0);
				mBV.push_back( std::pair<Vec3, Vec3>( Vec3(0., 0., 0. ), Vec3( 0., 0., 0. ) ) );
				calcBV( mFPolys.size()-1 );

				// Recalc front angles
				//std::cout << "Here\n";
				for( std::list<int>::iterator it2 = mFPolys.front().begin(); it2 != mFPolys.front().end(); ++it2 )
				{
					updateFrontAngles( it2 );
				}
				return false;

				//std::cout << "First half\n";
				//for( std::list<int>::iterator i = tmp1.begin(); i != tmp1.end(); ++i )
				//	std::cout << *i << std::endl;
				//std::cout << "Second half\n";
				//for( std::list<int>::iterator i = tmp2.begin(); i != tmp2.end(); ++i )
				//	std::cout << *i << std::endl;
			}
		}
	}
	return true;
}

void ImplTri::getInterDistNeigh( std::list<int>::iterator &v, std::list<int> &neigh )
{
	neigh.clear();

	if( mFPolys.front().size() < 6 )
		return;

	neigh.insert( neigh.end(), v, mFPolys.front().end() );
	neigh.insert( neigh.end(), mFPolys.front().begin(), v );

	neigh.erase( neigh.begin() );
	neigh.erase( neigh.begin() );
	neigh.erase( neigh.begin() );

	neigh.erase( --neigh.end() );
	neigh.erase( --neigh.end() );
/*
	// copy the full list
	neigh = std::list<int>( mFPolys.front() );

	// find v
	std::list<int>::iterator u = find( neigh.begin(), neigh.end(), *v );

	// remove two items before v (circular)
	for( int i = 0; i < 2; ++i )
	{
		if( *u == neigh.front() )
			u = neigh.end();

		--u;

		//std::cout << "u = " << *u << "\n";
		//std::cout << "neigh.end = " << neigh.back() << "\n";
		neigh.erase( u );
	}

	// move back to v
	if( *u != *v )
	{
		u = neigh.begin();
	}
	//std::cout << "u = " << *u << "\n";

	// remove two items after v (circular)
	if( *u == neigh.back() )
		u = neigh.begin();
	else
		++u;

	for( int i = 0; i < 2; ++i )
	{
		if( *u == neigh.back() )
			u = neigh.begin();
		else
			++u;

		std::cout << "b u = " << *u << "\n";
		neigh.erase( u );
		std::cout << "a u = " << *u << "\n";
	}

	// Move back to v
	if( *v == neigh.back() )
		u = neigh.end();
	else
		--u;

	std::cout << "u = " << *u << "\n";

	neigh.erase( u );
	*/
}

void ImplTri::addTri( std::list<int>::iterator &mA, double delta, std::vector<int> &triangles )
{
			// How many triangles to add
			std::list<int>::iterator v1, v2;
			getNeighbors( mA, v1, v2 );
			//std::cout << "v1 = " << *v1 << ", mA = " << *mA << ", v2 = " << *v2 << "\n";
			int nt = (int) floor(3. * mPoints[*mA].mFrontAngle / M_PI) + 1;

			double delw = mPoints[*mA].mFrontAngle / nt;

			//std::cout << "w = " << mPoints[*mA].mFrontAngle << ", nt = " << nt << ", delw = " << delw << "\n"; 
	
			// Too small
			if( delw < 0.8 && nt > 1 )
			{
				--nt;
				delw = mPoints[*mA].mFrontAngle / nt;
			}
			// too big
			else if( nt == 1 && delw > .8 && 
				(mPoints[*v1].mPos - mPoints[*v2].mPos).length() > 1.2*delta)
			{
				nt = 2;
				delw *= .5;
			}
			// Too big
			else if( mPoints[*mA].mFrontAngle < 3. &&
				     (( mPoints[*v1].mPos - mPoints[*mA].mPos).length() < .5 * delta ||
					  ( mPoints[*v2].mPos - mPoints[*mA].mPos).length() < .5 * delta ))
			{
				nt = 1;
			}
	
			// Add new triangles
			std::list<int> newPs;
			newPs.clear();
			if( nt == 1 )
			{
				triangles.push_back( *v1 );
				triangles.push_back( *v2 );
				triangles.push_back( *mA );
			}
			else
			{
				double u, v, f;
				P &p = mPoints[*mA];
				u = p.mU;
				v = p.mV;
				f = p.mF;
				Vec3 p_i, n, t1, t2;
				Vec3 q_i = project( *mA, *v1 ); 
				//std::cout << "nt = " << nt << ", delw = " << delw << ", w = " << mPoints[*mA].mFrontAngle << "\n";
	
				Mat4 m;
				
				for( int i = 0; i < nt - 1; ++i )
				{
					// Perform rotation
					// Tralslate mA to origin
					q_i = q_i - p.mPos;
					// Rotate around the norm
					m = m.setRotateRad( delw, p.mNorm ); 
					q_i = m * q_i;
					// Translate back to correct pos
					q_i = q_i + p.mPos;
	
					surfaceNearPoint( q_i, p_i, n, t1, t2, u, v, f );
//					if( mPoints.size() - 1 == 44)//( p_i - Vec3(-0.156239,0.987719,0.273178)).length() < 0.001)
//					{
//						std::cout << q_i << "->" << p_i << "\n";
//						std::cout << "u, v = " << u << ", " << v << "\n";
//					}
				    //std::cout << "p_" << i+1 << " = " << p_i << "\n";
					mPoints.push_back( P( p_i, n, t1, t2, u, v, f ) );
					newPs.push_back( mPoints.size() - 1 );
				}

				// Add the triangles
				// The first
				//std::cout << "ma = " << *mA << << std::endl;
				std::list<int>::iterator i = newPs.begin();
				std::list<int>::iterator i1 = newPs.begin();
				++i1;

				triangles.push_back( *v1 );
				triangles.push_back( *i );
				triangles.push_back( *mA );
				// The middle
				while( i1 != newPs.end() ) {
					triangles.push_back( *i );
					triangles.push_back( *i1 );
					triangles.push_back( *mA );
					++i;
					++i1;
				}
				// The last
				triangles.push_back( *i );
				triangles.push_back( *v2 );
				triangles.push_back( *mA );
			}
	
	
			// update the front
			// delete mA
			mFPolys.front().erase( mA );
			// Add the new points
			mFPolys.front().splice( v2, newPs );
			// Recalc the required front angles, just do all for now but only should do those that need it!
			for( std::list<int>::iterator i = mFPolys.front().begin(); i != mFPolys.front().end(); ++i )
			{
				updateFrontAngles( i );
			}
			// recalculate the bounding volume
			calcBV( 0 );
}

	}
}

