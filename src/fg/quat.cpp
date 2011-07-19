#include "fg/quat.h"

#include <iostream>

namespace fg {
    Quat::Quat()
        : w( 1 )
        , v( 0, 0, 0 )
    {
    }

    Quat::Quat( const Quat &q )
        : w( q.w )
        , v( q.v )
    {
    }

    Quat::Quat( double aW, double x, double y, double z )
        : w( aW )
        , v( x, y, z )
    {
    }

    Quat::Quat( const Vec3 &axis, double radians )
    {
        set( axis, radians );
    }

    Quat::Quat( const Vec3 &from, const Vec3 &to )
    {
        set( from, to );
    }

    Quat::Quat( double xRotation, double yRotation, double zRotation )
    {
        set( xRotation, yRotation, zRotation );
    }

    Quat::Quat( const Mat4 &m )
    {
        set( m );
    }

    Quat::Quat( const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis )
    {
        set( xaxis, yaxis, zaxis );
    }

    Quat &Quat::operator=( const Quat &rhs )
    {
        v = rhs.v;
        w = rhs.w;
        return *this;
    }

    void Quat::set( const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis )
    {
        double kRot[16];
        kRot[0] = xaxis.getX();
        kRot[1] = xaxis.getY();
        kRot[2] = xaxis.getZ();
        kRot[3] = 0;
        kRot[4] = yaxis.getX();
        kRot[5] = yaxis.getY();
        kRot[6] = yaxis.getZ();
        kRot[7] = 0;
        kRot[8] = zaxis.getX();
        kRot[9] = zaxis.getY();
        kRot[10] = zaxis.getZ();
        kRot[11] = 0;
        kRot[12] = 0;
        kRot[13] = 0;
        kRot[14] = 0;
        kRot[15] = 1;
        set( Mat4( kRot ) );
    }

    void Quat::set( double aW, double x, double y, double z )
    {
        w = aW;
        v = Vec3( x, y, z );
    }

    void Quat::set( const Vec3 &from, const Vec3 &to )
    {
        Vec3 axis = from.cross( to );
        set( from.dot( to ), axis.getX(), axis.getY(), axis.getZ() );
        normalise();
        w += 1.0;

        if( w <= EPSILON ) {
            if( from.getZ() * from.getZ() > from.getX() * from.getX() ) {
                set( 0.0, 0.0, from.getZ(), -from.getY() );
            }
            else {
                set( 0.0, from.getY(), -from.getX(), 0.0 );
            }
        }

        normalise();
    }

    void Quat::set( const Vec3 &axis, double radians )
    {
        w = cos( radians * .5 );
        v = axis / axis.length() * sin( radians * .5 );
    }

    void Quat::set( double xRotation, double yRotation, double zRotation )
    {
        zRotation *= .5;
        yRotation *= .5;
        xRotation *= .5;
        // get sines and cosines of half angles
        double Cx = cos( xRotation );
        double Sx = sin( xRotation );
        double Cy = cos( yRotation );
        double Sy = sin( yRotation );
        double Cz = cos( zRotation );
        double Sz = sin( zRotation );
        // multiply it out
        w = Cx * Cy * Cz - Sx * Sy * Sz;
        v.setX( Sx * Cy * Cz + Cx * Sy * Sz );
        v.setY( Cx * Sy * Cz - Sx * Cy * Sz );
        v.setY( Cx * Cy * Sz + Sx * Sy * Cx );
    }

    void Quat::set( const Mat4 &m )
    {
        if( m.get( 0, 0 ) + m.get( 1, 1 ) + m.get( 2, 2 ) > 0. )
        {
            double t = m.get( 0, 0 ) + m.get( 1, 1 ) + m.get( 2, 2 ) + 1.;
            double s = 0.5 / sqrt( t );
            w = s * t;
            v.setX( ( m[2][1] - m[1][2] ) * s );
            v.setY( ( m[0][2] - m[2][0] ) * s );
            v.setZ( ( m[1][0] - m[0][1] ) * s );
        }
        else
        {
            unsigned int i = 0;

            if( m.get( 1, 1 ) > m.get( 0, 0 ) )
                i = 1;

            if( m.get( 2, 2 ) > m.get( i, i ) )
                i = 2;

            unsigned int j = ( i + 1 ) % 3;
            unsigned int k = ( j + 1 ) % 3;
            double s = sqrt( m.get( i, i ) - m.get( j, j ) - m.get( k, k ) + 1.0 );
            //(*this)[i] = 0.5 * s;

            if( i == 0 )
                v.setX( 0.5 * s );
            else if( i == 1 )
                v.setY( 0.5 * s );
            else
                v.setZ( 0.5 * s );

            double recip = 0.5 / s;
            w = ( m.get( k, j ) - m.get( j, k ) ) * recip;

            if( j == 0 )
                v.setX( ( m.get( j, i ) + m.get( i, j ) ) * recip );
            else if( j == 1 )
                v.setY( ( m.get( j, i ) + m.get( i, j ) ) * recip );
            else
                v.setZ( ( m.get( j, i ) + m.get( i, j ) ) * recip );

            if( k == 0 )
                v.setX( ( m.get( k, i ) + m.get( i, k ) ) * recip );
            else if( k == 1 )
                v.setY( ( m.get( k, i ) + m.get( i, k ) ) * recip );
            else
                v.setZ( ( m.get( k, i ) + m.get( i, k ) ) * recip );

            //(*this)[j] = ( m.get(j,i) + m.get(i,j) ) * recip;
            //(*this)[k] = ( m.get(k,i) + m.get(i,k) ) * recip;
        }
    }

    void Quat::normalise()
    {
    	double len = length();
    	if (len!=0) { // not the best way but .. meh
            w /= len;
            v = v / len;
        }
        else {
            w = 1.0;
            v = Vec3( 0., 0., 0. );
        }
    }

    double Quat::length() const
    {
        return sqrt( w * w + v.lengthSquared() );
    }

    double Quat::lengthSquared() const
    {
        return w * w + v.lengthSquared();
    }

    Quat Quat::lerp( double t, const Quat &end ) const
    {
        // get cos of "angle" between quaternions
        float cosTheta = dot( end );
        // initialize result
        Quat result = end * t;

        // if "angle" between quaternions is less than 90 degrees
        if( cosTheta >= EPSILON ) {
            // use standard interpolation
            result += *this * ( 1.0 - t );
        }
        else {
            // otherwise, take the shorter path
            result += *this * ( t - 1.0 );
        }

//  std::cout << t << std::endl;
//  std::cout << *this << std::endl;
//  std::cout << end << std::endl;
//  std::cout << result << std::endl;
        return result;
    }

    double Quat::dot( const Quat &quat ) const
    {
        return w * quat.w + v.dot( quat.v );
    }

    Vec3 Quat::rotate( const Vec3 &point ) const
    {
        return operator*( point );
    }

    const Quat Quat::operator*( const Quat &rhs ) const
    {
        return Quat( rhs.w * w - rhs.v.getX() * v.getX() - rhs.v.getY() * v.getY() - rhs.v.getZ() * v.getZ(),
                     rhs.w * v.getX() + rhs.v.getX() * w + rhs.v.getY() * v.getZ() - rhs.v.getZ() * v.getY(),
                     rhs.w * v.getY() + rhs.v.getY() * w + rhs.v.getZ() * v.getX() - rhs.v.getX() * v.getZ(),
                     rhs.w * v.getZ() + rhs.v.getZ() * w + rhs.v.getX() * v.getY() - rhs.v.getY() * v.getX() );
    }

    const Quat Quat::operator*( double rhs ) const
    {
        return Quat( w * rhs, v.getX() * rhs, v.getY() * rhs, v.getZ() * rhs );
    }

    Quat &Quat::operator+=( const Quat &rhs )
    {
        w += rhs.w;
        v = v + rhs.v;
        return *this;
    }

    Quat Quat::inverted() const
    {
        double qdot = this->dot( *this );
        return Quat( v * -1. / qdot, w / qdot );
    }

    void Quat::invert()
    {
        double qdot = this->dot( *this );
        set( v * -1. / qdot, w / qdot );
    }

    const Quat Quat::operator-( const Quat &rhs ) const
    {
        const Quat &lhs = *this;
        return Quat( lhs.w - rhs.w, lhs.v.getX() - rhs.v.getX(), lhs.v.getY() - rhs.v.getY(), lhs.v.getZ() - rhs.v.getZ() );
    }

// transform a vector by the quaternion
    Vec3 Quat::operator*( const Vec3 &vec ) const
    {
        double vMult = 2. * ( v.getX() * vec.getX() + v.getY() * vec.getY() + v.getZ() * vec.getZ() );
        double crossMult = 2. * w;
        double pMult = crossMult * w - 1.;
        return Vec3( pMult * vec.getX() + vMult * v.getX() + crossMult * ( v.getY() * vec.getZ() - v.getZ() * vec.getY() ),
                     pMult * vec.getY() + vMult * v.getY() + crossMult * ( v.getZ() * vec.getX() - v.getX() * vec.getZ() ),
                     pMult * vec.getZ() + vMult * v.getZ() + crossMult * ( v.getX() * vec.getY() - v.getY() * vec.getX() ) );
    }

    const Quat Quat::operator+( const Quat &rhs ) const
    {
        const Quat &lhs = *this;
        return Quat( lhs.w + rhs.w, lhs.v.getX() + rhs.v.getX(), lhs.v.getY() + rhs.v.getY(), lhs.v.getZ() + rhs.v.getZ() );
    }

// get axis-angle representation's axis
    Vec3 Quat::getAxis() const
    {
        double cos_angle = w;

        if( fabs( w - 1.) < EPSILON )
            return Vec3( 1., 0., 0. );

        double invLen = 1. / sqrt( 1.0 - cos_angle * cos_angle );
        return v * invLen;
    }

// get axis-angle representation's angle in radians
    double Quat::getAngle() const
    {
        double cos_angle = w;
        return acos( cos_angle ) * 2.;
    }

    Quat Quat::slerp( double t, const Quat &end ) const
    {
        // get cosine of "angle" between quaternions
        double cosTheta = this->dot( end );
        double startInterp, endInterp;

        // if "angle" between quaternions is less than 90 degrees
        if( cosTheta >= EPSILON ) {
            // if angle is greater than zero
            if( ( 1. - cosTheta ) > EPSILON ) {
                // use standard slerp
                double theta = acos( cosTheta );
                double recipSinTheta = 1. / sin( theta );
                startInterp = sin( ( 1. - t ) * theta ) * recipSinTheta;
                endInterp = sin( t * theta ) * recipSinTheta;
            }
            // angle is close to zero
            else {
                // use linear interpolation
                startInterp = 1. - t;
                endInterp = t;
            }
        }
        // otherwise, take the shorter route
        else {
            // if angle is less than 180 degrees
            if( ( 1. + cosTheta ) > EPSILON ) {
                // use slerp w/negation of start quaternion
                double theta = acos( -cosTheta );
                double recipSinTheta = 1. / sin( theta );
                startInterp = sin( ( t - 1. ) * theta ) * recipSinTheta;
                endInterp = sin( t * theta ) * recipSinTheta;
            }
            // angle is close to 180 degrees
            else {
                // use lerp w/negation of start quaternion
                startInterp = t - 1.0;
                endInterp = t;
            }
        }

        return *this * startInterp + end * endInterp;
    }

    Quat Quat::inverse() const
    {
        double norm = w * w + v.getX() * v.getX() + v.getY() * v.getY() + v.getZ() * v.getZ();
        double normRecip = 1. / norm;
        return Quat( normRecip * w, -normRecip * v.getX(), -normRecip * v.getY(), -normRecip * v.getZ() );
    }

    Mat4 Quat::toMat4() const
    {
    	// from: http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
    	double a = w, b = v[0], c = v[1], d = v[2];
    	Mat4 m = Mat4::Identity();

    	m.get(0,0) = a*a + b*b - c*c  - d*d;
    	m.get(0,1) = 2*b*c - 2*a*d;
    	m.get(0,2) = 2*b*d + 2*a*c;

    	m.get(1,0) = 2*b*c + 2*a*d;
    	m.get(1,1) = a*a - b*b + c*c - d*d;
    	m.get(1,2) = 2*c*d - 2*a*b;

    	m.get(2,0) = 2*b*d - 2*a*c;
    	m.get(2,1) = 2*c*d + 2*a*b;
    	m.get(2,2) = a*a - b*b - c*c + d*d;

    	return m;
    }
}

std::ostream &operator <<( std::ostream &oss, const fg::Quat &q )
{
    oss << q.getAxis() << " @ " << q.getAngle() * ( 180. / M_PI ) << "deg";
    return oss;
}

