#include "fg/vec3.h"

/*
 *
 vcg::Point3d& operator=(vcg::Point3d& p, const fg::Vec3& v){
	p.X() = v.X();
	p.Y() = v.Y();
	p.Z() = v.Z();
}*/

namespace fg{
	Vec3::Vec3():vcg::Point3d(){}

	Vec3::Vec3(double x, double y, double z)
	:vcg::Point3d(x,y,z)
	{}

	void Vec3::normalise(){
		double n = vcg::Norm(*this);
		*this /= n;
	}

//	double Vec3::dot(const Vec3 &otherV) const{
//		return
//	}

	Vec3 Vec3::operator+(const Vec3& b) const{
		return Vec3(this->X()+b.X(),this->Y()+b.Y(),this->Z()+b.Z());
	}

	Vec3 Vec3::operator-(const Vec3& b) const{
		return Vec3(this->X()-b.X(),this->Y()-b.Y(),this->Z()-b.Z());
	}

	Vec3 Vec3::operator*(double d) const{
		return Vec3((vcg::Point3d)(*this) * d);
	}

}

std::ostream& operator<<(std::ostream& o, const fg::Vec3& v){
	o << "{" << v.X() << "," << v.Y() << "," << v.Z() << "}";
}
