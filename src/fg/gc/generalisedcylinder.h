#ifndef FG_GC_GENERALISED_CYLINDER_H
#define FG_GC_GENERALISED_CYLINDER_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <utility>

#include "fg/fg.h"
#include "fg/quat.h"

#include "fg/gc/interpolator.h"
#include "fg/gc/carriercurve.h"
#include "fg/gc/crosssection.h"

namespace fg {
    namespace gc {
        /**
         * \brief Uses a CarrierCurve and a CrossSection to define a surface.
         */
        class GeneralisedCylinder {
            public:
        		GeneralisedCylinder( const CarrierCurve &carrier, const std::vector<Mat4> &orients, const CrossSection &crossSection,
								     const std::vector< std::pair<double,double> > &csDomains,
									 const Interpolator<double> &scale,
									 const std::vector< std::pair<double,double> > &scaleDomains );
        		GeneralisedCylinder( const CarrierCurve &carrier, const std::vector<Mat4> &orients, const CrossSection &crossSection,
								     const std::vector< std::pair<double,double> > &csDomains,
									 const Interpolator<double> &scale,
									 const std::vector< std::pair<double,double> > &scaleDomains, const std::vector< int > &strips );
                Vec3 getPosition( double u, double v ) const;
				Vec3 getDerivativeU( double u, double v ) const;
				Vec3 getDerivativeV( double u, double v ) const;
				Vec3 getNorm( double u, double v ) const;
				Vec3 getNormU( double u, double v ) const;
				Vec3 getNormV( double u, double v ) const;
				double unNormalise( double u, double v ) const;
                int createMesh( Mesh::MeshBuilder &mb ) const;

				void impl( Vec3 x, double &u_i, double &v_i, double &d, Vec3 &grad ) const;

				void sim(double &x, double &y, double &z,
        		         double a, double b, double c, double d,
        		      	 double l, double m, double n, double k,
        		         double p, double q, double r, double s) const;

            protected:
                const CarrierCurve &mCarrier;
                const CrossSection &mCrossSection;
				const Interpolator<double> &mScale;
                std::vector< std::pair<Quat, Quat> > mOrients;
				std::vector< std::pair<double, double> > mCSDomain;
				std::vector< std::pair<double, double> > mScaleDomain;
				std::vector< int > mStrips;
				double getTotalLengthSquared( const std::vector< Vec3 > &polyLine ) const;
				const static int mMaxIts = 100;
				const static double mThreshHold = 1E-30;

                void updateOrients( const std::vector<Quat> &orients );
				double getCrossSectionV( double v ) const;
				double getScaleV( double v ) const;
                Quat orient( double v ) const;
        };

    }
}
#endif
