#include "fg/vec3.h"

/*
 *
 vcg::Point3d& operator=(vcg::Point3d& p, const fg::Vec3& v){
	p.X() = v.X();
	p.Y() = v.Y();
	p.Z() = v.Z();
}*/

std::ostream& operator<<(std::ostream& o, const fg::Vec3& v){
	o << "{" << v.X() << "," << v.Y() << "," << v.Z() << "}";
}
