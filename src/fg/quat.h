#ifndef FG_QUAT
#define FG_QUAT

#include "fg/fg.h"
#include "fg/mat4.h"
#include "fg/vec3.h"

namespace fg {
	/**
	 * A class to store rotation as quaternion.
	 */
	class Quat {
		public:
		  Vec3			v; 
		  double		w;
		  Quat();
		  Quat(const Quat &other);
		  Quat & operator=(const Quat &other);
		  Quat( double aW, double x, double y, double z );
		  Quat( const Vec3 &axis, double radians );
		  Quat( const Vec3 &from, const Vec3 &to );
		  Quat( double xRotation, double yRotation, double zRotation );
		  Quat( const Mat4 &m );

		  void set( double aW, double x, double y, double z );
		  void set( const Vec3 &from, const Vec3 &to );
		  void set( const Vec3 &axis, double radians );
		  void set( double xRotation, double yRotation, double zRotation );
		  void set( const Mat4 &m );

		  void normalise();

		  double length() const;
		  double lengthSquared() const;

		  Quat lerp( double t, const Quat &end ) const;

		  double dot( const Quat &quat ) const;

          const Quat operator*( double rhs ) const;
		  Vec3 operator*( const Vec3 &vec ) const;
          const Quat operator-( const Quat &rhs ) const;
          Quat& operator+=( const Quat &rhs );

          Quat inverted() const;
		  void invert();
		private:
			static const double EPSILON = 4.37114e-05;
	};
}
#endif
