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


	public:
		static Mat4 Identity();
	};
};

std::ostream& operator<<(std::ostream& o, const fg::Mat4& m);

#endif

