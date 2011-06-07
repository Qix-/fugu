#ifndef FG_VEC3_H
#define FG_VEC3_H

#include <ostream>

#include <vcg/space/point3.h>

namespace fg {
	// typedef vcg::Point3d Vec3;

	class Vec3: public vcg::Point3d {






		// interface for lua bindings
		public:
			double getX(){return X();}
			void setX(double x){X() = x;}
			double getY(){return Y();}
			void setY(double y){Y() = y;}
			double getZ(){return Z();}
			void setZ(double z){Z() = z;}
	};

	/*
	class Vec3 {
		// TODO?
	}
	*/
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Vec3&);

#endif
