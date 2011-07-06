#ifndef FG_MAT4_H
#define FG_MAT4_H

#include <ostream>

#include "fg/vec3.h"

#include <vcg/math/matrix44.h>

namespace fg {
	/**
	 * /brief 4x4 Matrix
	 */
	class Mat4: public vcg::Matrix44<double> {
	public:
		Mat4();
		Mat4(const vcg::Matrix44<double>& m);

		/// Access a matrix element (row,col)
		double& get(int r, int c);

		/// Access a const matrix element (row,col)
		double get(int r, int c) const;

		//Vec3 operator*(const Vec3& v) const;

		/// Set this matrix to be a translation matrix
		void setTranslate(double x, double y, double z);
		/// Set this matrix to be a rotation matrix (radians, axis)
		void setRotateRad(double radians, double x, double y, double z);

	public:
		static Mat4 Identity();
		static Mat4 Zero();
	};
};

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m);

#endif

