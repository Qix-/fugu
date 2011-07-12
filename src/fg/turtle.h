#ifndef FG_GC_TURTLE_H
#define FG_GC_TURTLE_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "fg/fg.h"

#include "fg/carriercurvelinear.h"
#include "fg/crosssectioncircular.h"
#include "fg/generalisedcylinder.h"

namespace fg {
	namespace gc {
	class Turtle {
		public:
		  Turtle();
	      ~Turtle();
		  void move(double distance);
		  void yaw(double theta);
		  void pitch(double theta);
		  void roll(double theta);
		  void beginCylinder();
          void addPoint();
		  void endCylinder();
		  boost::shared_ptr<Mesh> getMesh(int i, int n = 10, int m = 10);

		protected:
		  Mat4 mFrame;
		  std::vector<Mat4> mPrevFrames;
		  std::vector<const CarrierCurve*> mCarriers;
		  std::vector<const CrossSection*> mCrossSections;
		  std::vector<const GeneralisedCylinder*> mCylinders;
	};
	}
}

#endif
