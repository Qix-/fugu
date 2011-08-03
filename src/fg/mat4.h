/**
 * \file
 * \brief
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

#ifndef FG_MAT4_H
#define FG_MAT4_H

#include <ostream>

#include "fg/vec3.h"

#include <vcg/math/matrix44.h>

namespace fg {
	/**
	 * \brief A 4x4 Matrix
	 */
	class Mat4: public vcg::Matrix44<double> {
	public:
		Mat4();
		Mat4(const vcg::Matrix44<double>& m);
		Mat4(const double v[]);

		/// \brief Access a matrix element (row,col)
		double& get(int r, int c);

		/// \brief Access a const matrix element (row,col)
		double get(int r, int c) const;

		//Vec3 operator*(const Vec3& v) const;

		/// \brief Set this matrix to be a translation transform
		void setTranslate(double x, double y, double z);
		/// \brief Set this matrix to be a translation transform
		void setTranslate(Vec3 t);
		/// \brief Set this matrix to be a rotation transform (radians, axis)
		void setRotateRad(double radians, double x, double y, double z);
		/// \brief Set this matrix to be a scaling transform
		void setScale(double sx, double sy, double sz);

		void set(const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis);

		// operators
		Mat4 operator+(const Mat4 &m) const;
		Mat4 operator-(const Mat4 &m) const;
		Mat4 operator*(const Mat4 &m) const;

		// Vec3 operator*(const Vec3 &v) const; // this is defined at global scope

		bool operator==(const  Mat4 &m) const;
		bool operator!= (const  Mat4 &m) const;

		Mat4 operator-() const;
		Mat4 operator*(const double k) const;
		void operator+=(const Mat4 &m);
		void operator-=(const Mat4 &m);
		void operator*=( const Mat4 & m );
		void operator*=( const double k );

	public:
		static Mat4 Identity();
		static Mat4 Zero();
	};
};

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m);

#endif

