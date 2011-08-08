#ifndef FG_GC_CROSS_SECTION_CIRCULAR_H
#define FG_GC_CROSS_SECTION_CIRCULAR_H

#include "fg/fg.h"

#include "fg/gc/crosssection.h"

namespace fg {
    namespace gc {
        /**
         * \brief Provides a variyng cross section function.
         */
        class CrossSectionCircular : public CrossSection {
            public:
                CrossSectionCircular( double radius = 1. );
                virtual Vec3 getPosition( double u, double v ) const;
                virtual Vec3 getDerivativeU( double u, double v ) const;
                virtual Vec3 getDerivativeV( double u, double v ) const;

                virtual std::vector<Vec3> getCrossSection( double v, double scale = 1. ) const;
            protected:
                double mRadius;
                static const int SEGS = 8;
        };

    }
}

#endif
