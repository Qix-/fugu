#include "fg/vec3.h"

std::ostream& operator<<(std::ostream& o, const fg::Vec3& v){
	o << "{" << v.X() << "," << v.Y() << "," << v.Z() << "}";
}
