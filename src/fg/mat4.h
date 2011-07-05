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

		///brief Access a matrix element (row,col)
		double& get(int r, int c);

		///brief Access a const matrix element (row,col)
		double get(int r, int c) const;

		//Vec3 operator*(const Vec3& v) const;
	public:
		static Mat4 Identity();
		static Mat4 Zero();
	};
};

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m);

#endif

