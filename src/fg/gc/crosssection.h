#ifndef FG_GC_CROSS_SECTION_H
#define FG_GC_CROSS_SECTION_H

#include <vector>

#include "fg/fg.h"

namespace fg {
    namespace gc {
        /**
         * \brief Provides a variyng cross section function.
         */
        class CrossSection {
            public:
                virtual Vec3 getPosition( double u, double v ) const = 0;
                virtual Vec3 getDerivativeU( double u, double v ) const = 0;
                virtual Vec3 getDerivativeV( double u, double v ) const = 0;
				virtual double unNormalise( double u, double v ) const = 0;

				virtual std::vector<Vec3> getCrossSection( double v, double scale = 1. ) const = 0;
            protected:
        };

    }
}
#endif
