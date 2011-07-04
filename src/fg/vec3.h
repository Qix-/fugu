#ifndef FG_VEC3_H
#define FG_VEC3_H

#include <ostream>

#include <vcg/space/point3.h>

namespace fg {
	// typedef vcg::Point3d Vec3;

	class Vec3: public vcg::Point3d {
		public:
			Vec3(const vcg::Point3d& p){
				vcg::Point3d();
				X() = p.X();
				Y() = p.Y();
				Z() = p.Z();
			}

			Vec3();
			Vec3(double x, double y, double z);

		// interface for lua bindings
		public:
			double getX() const {return X();}
			void setX(double x){X() = x;}
			double getY() const {return Y();}
			void setY(double y){Y() = y;}
			double getZ() const {return Z();}
			void setZ(double z){Z() = z;}

			void normalise();

			Vec3 operator+(const Vec3& b) const;
			Vec3 operator-(const Vec3& b) const;
			Vec3 operator*(double d) const;
	};
}

// conversion
//vcg::Point3d& operator=(vcg::Point3d& p, const fg::Vec3& v);


// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Vec3&);

#endif
