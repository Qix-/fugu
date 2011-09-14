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

		// Column-wise construction
		Mat4(	double c1r1, double c1r2, double c1r3, double c1r4,
				double c2r1, double c2r2, double c2r3, double c2r4,
				double c3r1, double c3r2, double c3r3, double c3r4,
				double c4r1, double c4r2, double c4r3, double c4r4);

		static Mat4 Identity();
		static Mat4 Zero();

		// Accessors
		/// \brief Access a matrix element (row,col)
		double& get(int r, int c);
		/// \brief Access a const matrix element (row,col)
		double get(int r, int c) const;


		// operators
		Mat4 operator+(const Mat4 &m) const;
		Mat4 operator-(const Mat4 &m) const;
		Mat4 operator*(const Mat4 &m) const;

		// Vec3 operator*(const Vec3 &v) const; // this is defined at global scope

		bool operator==(const  Mat4 &m) const;
		bool operator!= (const  Mat4 &m) const;

		Mat4 operator-() const;
		Mat4 operator*(const double k) const;
		friend Vec3 operator*(const Vec3& a, const Mat4& b);

		void operator+=(const Mat4 &m);
		void operator-=(const Mat4 &m);
		void operator*=( const Mat4 & m );
		void operator*=( const double k );

		// Transformation functions
		/// \brief Set this matrix basis to consist of the supplied axes
		Mat4& setBasis(const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis);
		/// \brief Set this matrix to be a translation transform
		Mat4& setTranslate(double x, double y, double z);
		/// \brief Set this matrix to be a translation transform
		Mat4& setTranslate(const Vec3& t);
		/// \brief Set this matrix to be a rotation transform (radians, axis)
		Mat4& setRotateRad(double radians, double x, double y, double z);
		/// \brief Set this matrix to be a rotation transform (radians, axis)
		Mat4& setRotateRad(double radians, const Vec3& axis);
		/// \brief Set this matrix to be a rotation transform (fromDir, toDir)
		Mat4& setRotate(const Vec3& from, const Vec3& to);

		/// \brief Set this matrix to be a scaling transform
		Mat4& setScale(double sx, double sy, double sz);
		/// \brief Set this matrix to be a uniform scaling transform
		Mat4& setScale(double s);
		/// \brief Set this matrix to be a scaling transform
		Mat4& setScale(const Vec3& scale);

		/// \deprecated use setBasis (its intention is clearer)
		void set(const Vec3 &xaxis, const Vec3 &yaxis, const Vec3 &zaxis);
	};

	// other operators
	Vec3 operator*(const Vec3& a, const Mat4& b);
};

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m);

#endif

