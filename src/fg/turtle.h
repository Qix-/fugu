#ifndef FG_GC_TURTLE_H
#define FG_GC_TURTLE_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stack>

#include "fg/fg.h"

#include "fg/lincarrier.h"
#include "fg/pbezinterpdiv.h"
#include "fg/pbezcarrier.h"
#include "fg/crosssectioncircular.h"
#include "fg/scaleinterpcrosssec.h"
#include "fg/generalisedcylinder.h"

namespace fg {
    namespace gc {
        class Turtle {
            public:
				static const int LINEAR_CARRIER = 0;
				static const int BEZIER_CARRIER = 1;
				static const int INTERPOLATOR_CROSS_SECTION = 0;
				static const int SCALED_INTERPOLATOR_CROSS_SECTION = 1;
                Turtle();
                ~Turtle();
                void move( double distance );
                void yaw( double theta );
                void pitch( double theta );
                void roll( double theta );
                void beginCylinder();
                void addPoint();
                void endCylinder();
				void setScale(double scale);
				void pushState();
				void popState();
                boost::shared_ptr<Mesh> getMesh( int n = 10, int m = 10 );

            protected:
				// The state
				struct TurtleState
				{
					int carrierMode;
					int crossSectionMode;
					int currentCS;
					double scale;
                	Mat4 frame;
				};
				TurtleState mState;

				// Stack of states
				std::stack<TurtleState> mStateStack;
                // Data to be used for the next cylinder
				std::vector<double> mScaleArr;
				std::vector< std::pair<double, double> > mDomains;
                std::vector<Mat4> mPrevFrames;

				// Storage of previous cylinders
				std::vector<spline::Interpolator<Vec3> *> mCrossSecLibrary;
                std::vector<const CarrierCurve *> mCarriers;
                std::vector<const CrossSection *> mCrossSections;
                std::vector<const GeneralisedCylinder *> mCylinders;
				std::vector<const spline::Interpolator<double> *> mScalers;
        };
    }
}

#endif
