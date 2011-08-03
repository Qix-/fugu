/**
 * \file
 * \author ben
 * 
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___     
 *   |  _|___ 
 *   |  _| . | fg: real-time procedural 
 *   |_| |_  | animation and generation 
 *       |___| of 3D forms
 *
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in 
 *   the LICENSE file.
 * 
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#include "fg/mat4.h"
#include <iomanip>

namespace fg {
	Mat4::Mat4():vcg::Matrix44<double>(){}

	Mat4::Mat4(const vcg::Matrix44<double>& m):vcg::Matrix44<double>(m){}

	Mat4::Mat4(const double v[]):vcg::Matrix44<double>(v){}

	void Mat4::set(const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis){
        double kRot[16];
        kRot[0] = xaxis.getX();
        kRot[4] = xaxis.getY();
        kRot[8] = xaxis.getZ();
        kRot[12] = 0;
        kRot[1] = yaxis.getX();
        kRot[5] = yaxis.getY();
        kRot[9] = yaxis.getZ();
        kRot[13] = 0;
        kRot[2] = zaxis.getX();
        kRot[6] = zaxis.getY();
        kRot[10] = zaxis.getZ();
        kRot[14] = 0;
        kRot[3] = 0;
        kRot[7] = 0;
        kRot[11] = 0;
        kRot[15] = 1;

		(*this) = Mat4(kRot);
	}

	double& Mat4::get(int r, int c){
		return ElementAt(r,c);
	}

	double Mat4::get(int r, int c) const {
		return ElementAt(r,c);
	}

	/*
	Vec3 Mat4::operator*(const Vec3& v) const {
		return Vec3(*this * (vcg::Point3<double>)(v));
	}
	*/

	void Mat4::setTranslate(double x, double y, double z){
		vcg::Matrix44<double>::SetTranslate(x,y,z);
	}

	void Mat4::setTranslate(Vec3 t){
		vcg::Matrix44<double>::SetTranslate(t[0],t[1],t[2]);
	}

	void Mat4::setRotateRad(double radians, double x, double y, double z){
		vcg::Matrix44<double>::SetRotateRad(radians,vcg::Point3<double>(x,y,z));
	}

	void Mat4::setScale(double sx, double sy, double sz){
		vcg::Matrix44<double>::SetScale(sx,sy,sz);
	}

	// operators
	Mat4 Mat4::operator+(const Mat4 &m) const {return vcg::Matrix44<double>::operator+(m);}
	Mat4 Mat4::operator-(const Mat4 &m) const {return vcg::Matrix44<double>::operator-(m);}
	Mat4 Mat4::operator*(const Mat4 &m) const {return vcg::Matrix44<double>::operator*(m);}

	/*
	Vec3 Mat4::operator*(const Vec3 &v) const {
		vcg::Point4<double> p = vcg::Matrix44<double>::operator*(vcg::Point4<double>(v.X(),v.Y(),v.Z(),1)).HomoNormalize();
		return Vec3(p.X(),p.Y(),p.Z());
	}
	*/

	bool Mat4::operator==(const  Mat4 &m) const {return vcg::Matrix44<double>::operator==(m);}
	bool Mat4::operator!= (const  Mat4 &m) const {return vcg::Matrix44<double>::operator!=(m);}

	Mat4 Mat4::operator-() const {return vcg::Matrix44<double>::operator-();}
	Mat4 Mat4::operator*(const double k) const {return vcg::Matrix44<double>::operator*(k);}
	void Mat4::operator+=(const Mat4 &m) {return vcg::Matrix44<double>::operator+=(m);}
	void Mat4::operator-=(const Mat4 &m) {return vcg::Matrix44<double>::operator-=(m);}
	void Mat4::operator*=( const Mat4 & m ) {return vcg::Matrix44<double>::operator*=(m);}
	void Mat4::operator*=( const double k ) {return vcg::Matrix44<double>::operator*=(k);}

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
