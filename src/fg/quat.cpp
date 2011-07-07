#include "quat.h"


namespace fg {
Quat::Quat()
    :w( 1 )
    ,v( 0, 0, 0 )
{
}

Quat::Quat( const Quat& q )
    :w( q.w )
    ,v( q.v )
{
}

Quat::Quat( double aW, double x, double y, double z )
    :w( aW )
    ,v( x, y, z )
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

Quat& Quat::operator=( const Quat &rhs )
{
	v = rhs.v;
	w = rhs.w;
	return *this;
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
        if ( from.getZ() * from.getZ() > from.getX() * from.getX() ) {
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
    w = cos( radians / 2. );
    v = axis / axis.length() * sin( radians / 2. );
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
    w = Cx*Cy*Cz - Sx*Sy*Sz;
    v.setX( Sx*Cy*Cz + Cx*Sy*Sz );
    v.setY( Cx*Sy*Cz - Sx*Cy*Sz );
    v.setY( Cx*Cy*Sz + Sx*Sy*Cx );
}

void Quat::set( const Mat4 &m )
{
    double trace =  m.get(0,0) + m.get(2,2) + m.get(3,3);
    if ( trace > 0. )
    {
        double s = sqrt( trace + 1.0 );
        w = s * 0.5;
        double recip = 0.5 / s;
        v.setX( ( m.get(2,1) - m.get(1,2) ) * recip );
        v.setY ( ( m.get(0,2) - m.get(2,0) ) * recip );
        v.setZ ( ( m.get(1,0) - m.get(0,1) ) * recip );
    }
    else
    {
        unsigned int i = 0;
        if( m.get(1,1) > m.get(0,0) )
            i = 1;
        if( m.get(2,2) > m.get(i,i) )
            i = 2;
        unsigned int j = ( i + 1 ) % 3;
        unsigned int k = ( j + 1 ) % 3;
        double s = sqrt( m.get(i,i) - m.get(j,j) - m.get(k,k) + 1.0 );
        //(*this)[i] = 0.5 * s;

        if (i == 0)
            v.setX( 0.5 * s );
        else if (i == 1)
            v.setY( 0.5 * s );
        else
            v.setZ( 0.5 * s );

        double recip = 0.5 / s;
        w = ( m.get(k,j) - m.get(j,k) ) * recip;

        if (j == 0)
            v.setX( ( m.get(j,i) + m.get(i,j) ) * recip );
        else if (j == 1)
            v.setY( ( m.get(j,i) + m.get(i,j) ) * recip );
        else
            v.setZ( ( m.get(j,i) + m.get(i,j) ) * recip );

        if (k == 0)
            v.setX( ( m.get(k,i) + m.get(i,k) ) * recip );
        else if (k == 1)
            v.setY( ( m.get(k,i) + m.get(i,k) ) * recip );
        else
            v.setZ( ( m.get(k,i) + m.get(i,k) ) * recip );

        //(*this)[j] = ( m.get(j,i) + m.get(i,j) ) * recip;
        //(*this)[k] = ( m.get(k,i) + m.get(i,k) ) * recip;
    }
}

void Quat::normalise()
{
    if( double len = length() ) {
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
    return sqrt( w*w + v.lengthSquared() );
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

    return result;
}

double Quat::dot( const Quat &quat ) const
{
    return w * quat.w + v.dot( quat.v );
}

const Quat Quat::operator*( double rhs ) const
{
    return Quat( w * rhs, v.getX() * rhs, v.getY() * rhs, v.getZ() * rhs );
}

Quat& Quat::operator+=( const Quat &rhs )
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
    const Quat& lhs = *this;
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

}
