#ifndef FG_GC_TURTLE_H
#define FG_GC_TURTLE_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stack>

#include "fg/fg.h"

#include "fg/gc/lincarrier.h"
#include "fg/gc/pbezinterpdiv.h"
#include "fg/gc/pbezcarrier.h"
#include "fg/gc/crinterp.h"
#include "fg/gc/crosssectioncircular.h"
#include "fg/gc/scaleinterpcrosssec.h"
#include "fg/gc/interpcrosssec.h"
#include "fg/gc/generalisedcylinder.h"
#include "fg/gc/morphcrosssec.h"

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
                Vec3 getHeading();
                Vec3 getUp();
                boost::shared_ptr<Mesh> getMesh( );
				boost::shared_ptr<Mesh> getMesh( int gc );
				int getNumGC( );

				int getNumEndCaps() {
					return mEndCaps.size();
				}
				int getEndCap(int i) {
					if( i < 0 || i >= getNumEndCaps() )
						return -1;
					return mEndCaps[i];
				}

            //protected:
				// The state
				struct TurtleState
				{
					int carrierMode;
					int crossSectionMode;
					int currentCS;
					double scale;
					std::pair<double,double> stiffness;
                	Mat4 frame;

	                // Data to be used for the next cylinder/cross section
					std::vector<double> scaleArr;
					std::vector< std::pair<double, double> > scaleDomains;
					std::vector<int> crossSecArr;
					std::vector< std::pair<double, double> > crossSecDomains;
	                std::vector<Mat4> prevFrames;
					std::vector< std::pair<double, double> > stiffnessArr;
					std::vector<int> strips;
				};
				TurtleState mState;

				// Stack of states
				std::stack<TurtleState> mStateStack;

				// Storage of previous cylinders
				std::vector<PBezInterpDiv *> mCrossSecLibrary;
                std::vector<const CarrierCurve *> mCarriers;
                std::vector<const CrossSection *> mCrossSections;
                std::vector<const GeneralisedCylinder *> mCylinders;
				std::vector<const Interpolator<double> *> mScalers;
				std::vector<int> mEndCaps;
        };
    }
}

#endif
