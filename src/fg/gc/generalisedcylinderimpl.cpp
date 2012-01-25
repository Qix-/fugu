#include "fg/gc/generalisedcylinderimpl.h"

#include <iostream>
#include <cmath>

using namespace std;

namespace fg {
	namespace gc {
GeneralisedCylinderImpl::GeneralisedCylinderImpl( double radius )
{
	mRadius = radius;
}

/*
GeneralisedCylinderImpl::GeneralisedCylinderImpl( const CarrierCurve &carrier, const CrossSection &crossSection )
: mCarrier( carrier )
, mCrossSection( crossSection )
{
}

Vec3 GeneralisedCylinderImpl::getPosition( double u, double v ) const
{
    // Position on cross section
    Vec3 cs = mCrossSection.getPosition( u, v );
    // Carriers frame rotation
    Quat cr = mCarrier.orient( v );
    // Put it all together
    return mCarrier.getInterpolator()->getPosition( v ) + cr * cs;
}

Vec3 GeneralisedCylinderImpl::getDerU( double u, double v ) const
{
    // Carriers frame rotation
    Quat cr = mCarrier.orient( v );

	return cr * mCrossSection.getDerivativeU( u, v );
}

Vec3 GeneralisedCylinderImpl::getDerV( double u, double v ) const
{
    // Carriers frame rotation
    Quat cr = mCarrier.orient( v );

	return mCarrier.getInterpolator()->getDerivative( v ) + cr * mCrossSection.getDerivativeV( u, v );
}

Vec3 GeneralisedCylinderImpl::getNorm( double u, double v ) const
{
	return getDerU( u, v ).cross( getDerV( u, v ) );
}
*/
Vec3 GeneralisedCylinderImpl::getPosition( double u, double v ) const
{
	return Vec3( cos( 2.*M_PI*u), sin( 2.*M_PI*u), v );
}

Vec3 GeneralisedCylinderImpl::getDerU( double u, double v ) const
{
	return Vec3( -2.*M_PI*sin( 2.*M_PI*u), 2.*M_PI*cos( 2.*M_PI*u), 0. );
}

Vec3 GeneralisedCylinderImpl::getDerV( double u, double v ) const
{
	return Vec3( 0., 0., 1. );
}

Vec3 GeneralisedCylinderImpl::getNorm( double u, double v ) const
{
	Vec3 n = getPosition( u, v );
	n.setZ( 0. );
	return n;
}

Vec3 GeneralisedCylinderImpl::getNormU( double u, double v ) const
{
	double t = 1E-9;
	Vec3 n1 = getNorm( u - t, v );
	Vec3 n2 = getNorm( u + t, v );

	return (n2 - n1) * 5E8;
}

Vec3 GeneralisedCylinderImpl::getNormV( double u, double v ) const
{
	double t = 1E-9;
	Vec3 n1 = getNorm( u, v - t );
	Vec3 n2 = getNorm( u, v + t );

	return (n2 - n1) * 5E8;
}

double GeneralisedCylinderImpl::impl( Vec3 x, double &u_i, double &v_i ) const
{
	double d_i = 0.;
	double du, dv, dd;
	Vec3 su, sv, sd, s;

	for( int i = 0; i < mMaxIts; ++i )
	{
		s = getPosition( u_i, v_i );
		s = s + getNorm( u_i, v_i ) * d_i;
		su = getDerU( u_i, v_i ) + getNormU( u_i, v_i ) * d_i;
		sv = getDerV( u_i, v_i ) + getNormV( u_i, v_i ) * d_i;
		sd = getNorm( u_i, v_i );

//		std::cout << "u_i = " << u_i << std::endl;
//		std::cout << "v_i = " << v_i << std::endl;
//		std::cout << "d_i = " << d_i << std::endl;
//		std::cout << "b = " << getPosition( u_i, v_i ) << std::endl;
//		std::cout << "n = " << getNorm( u_i, v_i ) << std::endl;
//		std::cout << "s = " << s << std::endl;
//		std::cout << "s_u = " << su << std::endl;
//		std::cout << "s_v = " << sv << std::endl;
//		std::cout << "s_d = " << sd << std::endl;
//		std::cout << "n_u = " << getNormU( u_i, v_i ) << std::endl;
//		std::cout << "n_v = " << getNormV( u_i, v_i ) << std::endl;

		s = x - s;
		sim( du, dv, dd, 
		     su.getX(), sv.getX(), sd.getX(), s.getX(),
			 su.getY(), sv.getY(), sd.getY(), s.getY(),
			 su.getZ(), sv.getZ(), sd.getZ(), s.getZ() );

//		std::cout << "du = " << du << std::endl;
//		std::cout << "dv = " << dv << std::endl;
//		std::cout << "dd = " << dd << std::endl << std::endl;

		u_i = u_i + du;
		v_i = v_i + dv;
		d_i = d_i + dd;

		if( fabs( dd ) < mThreshHold )
			break;
	}

	return d_i;
}

void GeneralisedCylinderImpl::impl( Vec3 x, double &u_i, double &v_i, double &d, Vec3 &grad ) const
{
	double d_i = 0.;
	double du, dv, dd;
	Vec3 su, sv, sd, s;

	for( int i = 0; i < mMaxIts; ++i )
	{
		s = getPosition( u_i, v_i );
		s = s + getNorm( u_i, v_i ) * d_i;
		su = getDerU( u_i, v_i ) + getNormU( u_i, v_i ) * d_i;
		sv = getDerV( u_i, v_i ) + getNormV( u_i, v_i ) * d_i;
		sd = getNorm( u_i, v_i );

		s = x - s;
		sim( du, dv, dd, 
		     su.getX(), sv.getX(), sd.getX(), s.getX(),
			 su.getY(), sv.getY(), sd.getY(), s.getY(),
			 su.getZ(), sv.getZ(), sd.getZ(), s.getZ() );

		u_i = u_i + 1.*du;
		v_i = v_i + 1.*dv;
		d_i = d_i + 1.*dd;

		if( fabs( dd ) < mThreshHold )
			break;
	}

	d = d_i;
	grad = getNorm( u_i, v_i );
}

// Solves 3x3 sim eqs via cramers rule
void GeneralisedCylinderImpl::sim(double &x, double &y, double &z,
                              double a, double b, double c, double d,
                          	  double l, double m, double n, double k,
                          	  double p, double q, double r, double s) const
{
//	std::cout << a << " " << b << " " << c << " = " << d << std::endl;
//	std::cout << l << " " << m << " " << n << " = " << k << std::endl;
//	std::cout << p << " " << q << " " << r << " = " << s << std::endl;
	// Determinant
	double D = (a*m*r+b*p*n+c*l*q)-(a*n*q+b*l*r+c*m*p);

	//std::cout << "det = " << D << endl;

	// Solutions
	x = -((b*r*k+c*m*s+d*n*q)-(b*n*s+c*q*k+d*m*r))/D;
	y = -((a*n*s+c*p*k+d*l*r)-(a*r*k+c*l*s+d*n*p))/D;
	z = -((a*q*k+b*l*s+d*m*p)-(a*m*s+b*p*k+d*l*q))/D;
}
	}
}
