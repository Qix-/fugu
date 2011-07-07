#ifndef FG_GC_TURTLE_H
#define FG_GC_TURTLE_H

#include <vector>

#include "fg/fg.h"

#include "fg/carriercurvelinear.h"

namespace fg {
	namespace gc {
	class Turtle {
		public:
		  Turtle();
		  void move(double distance);
		  void beginCylinder();
          void addPoint();
		  GeneralisedCylinder endCylinder();

		protected:
		  Mat4 mFrame;
		  std::vector<Mat4> mPrevFrames;
	};
	}
}

#endif
