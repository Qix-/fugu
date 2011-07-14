#ifndef FG_GC_GENERALISED_CYLINDER_H
#define FG_GC_GENERALISED_CYLINDER_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <utility>

#include "fg/fg.h"
#include "fg/interpolator.h"
#include "fg/carriercurve.h"
#include "fg/crosssection.h"
#include "fg/quat.h"

namespace fg {
    namespace gc {
        /**
         * \brief Provides a variyng cross section function.
         */
        class GeneralisedCylinder {
            public:
                GeneralisedCylinder( const CarrierCurve &carrier, const CrossSection &crossSection, const std::vector<Mat4> &orients );
                Vec3 getPosition( double u, double v ) const;
                boost::shared_ptr<Mesh> createMesh( int n = 10, int m = 10 ) const;

            protected:
                const CarrierCurve &mCarrier;
                const CrossSection &mCrossSection;
                std::vector< std::pair<Quat, Quat> > mOrients;

                void updateOrients( const std::vector<Quat> &orients );
                Quat orient( double v ) const;
        };

    }
}
#endif
