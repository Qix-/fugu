#ifndef FG_GC_TURTLE_H
#define FG_GC_TURTLE_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stack>

#include "fg/fg.h"

#include "fg/lincarrier.h"
#include "fg/pbezinterpdiv.h"
#include "fg/pbezcarrier.h"
#include "fg/crinterp.h"
#include "fg/crosssectioncircular.h"
#include "fg/scaleinterpcrosssec.h"
#include "fg/interpcrosssec.h"
#include "fg/generalisedcylinder.h"
#include "fg/morphcrosssec.h"

namespace fg {
    namespace gc {
        class Turtle {
            public:
				static const int LINEAR_CARRIER = 0;
				static const int BEZIER_CARRIER = 1;
				static const int INTERPOLATOR_CROSS_SECTION = 0;
				static const int MORPH_CROSS_SECTION = 1;
                Turtle();
                ~Turtle();
				void setFrame( Vec3 pos, Vec3 heading, Vec3 up);
				void setFrameRel( Vec3 pos, Vec3 heading, Vec3 up);
                void move( double distance );
                void yaw( double theta );
                void pitch( double theta );
                void roll( double theta );
                void beginCylinder();
                void addPoint();
                void addPoint(int n);
                void endCylinder();
                void endCylinder(int n);
				void setCrossSection(int index);
				void setScale(double scale);
				void setStiffness( double s1, double s2 );
				void pushState();
				void popState();
				void setCarrierMode( int carrierMode );
				void setCrossSectionMode( int crossSectionMode );
				void beginCrossSection();
				int endCrossSection();
				Vec3 getPosition();
                boost::shared_ptr<Mesh> getMesh( );

            protected:
				// The state
				struct TurtleState
				{
					int carrierMode;
					int crossSectionMode;
					int currentCS;
					double scale;
					std::pair<double,double> stiffness;
                	Mat4 frame;
				};
				TurtleState mState;

				// Stack of states
				std::stack<TurtleState> mStateStack;

                // Data to be used for the next cylinder/cross section
				std::vector<double> mScaleArr;
				std::vector< std::pair<double, double> > mScaleDomains;
				std::vector<int> mCrossSecArr;
				std::vector< std::pair<double, double> > mCrossSecDomains;
                std::vector<Mat4> mPrevFrames;
				std::vector< std::pair<double, double> > mStiffnessArr;
				std::vector<int> mStrips;

				// Storage of previous cylinders
				std::vector<spline::PBezInterpDiv *> mCrossSecLibrary;
                std::vector<const CarrierCurve *> mCarriers;
                std::vector<const CrossSection *> mCrossSections;
                std::vector<const GeneralisedCylinder *> mCylinders;
				std::vector<const spline::Interpolator<double> *> mScalers;
        };
    }
}

#endif
