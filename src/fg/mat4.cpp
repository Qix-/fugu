#include "fg/mat4.h"
#include <iomanip>

namespace fg {
	Mat4::Mat4():vcg::Matrix44<double>(){}

	Mat4::Mat4(const vcg::Matrix44<double>& m):vcg::Matrix44<double>(m){}

	double& Mat4::get(int r, int c){
		return ElementAt(r,c);
	}

	double Mat4::get(int r, int c) const {
		return ElementAt(r,c);
	}

	// Static
	Mat4 Mat4::Identity(){return vcg::Matrix44<double>::Identity();}
	Mat4 Mat4::Zero(){Mat4 m; m.SetZero(); return m;}
}

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m){
	for(int r=0;r<4;r++){
		for(int c=0;c<4;c++){
			o << std::setprecision(3) << m.ElementAt(r,c);
			if (c<3) o << ", ";
		}
		o << "\n";
	}
	return o;
}
