#include "fg/gc/crosssectioncircular.h"

using namespace std;

namespace fg {
    namespace gc {

        CrossSectionCircular::CrossSectionCircular( double radius )
        {
            mRadius = radius;
        }
        Vec3 CrossSectionCircular::getPosition( double u, double v ) const
        {
            return Vec3( mRadius * cos( u ), mRadius * sin( u ), 0. );
        }
        Vec3 CrossSectionCircular::getDerivativeU( double u, double v ) const
        {
            return Vec3( -mRadius * sin( u ), mRadius * cos( u ), 0. );
        }
        Vec3 CrossSectionCircular::getDerivativeV( double u, double v ) const
        {
            return Vec3( 0., 0., 0. );
        }

        vector<Vec3> CrossSectionCircular::getCrossSection( double v, double scale ) const
        {
            vector<Vec3> ret;
            double u = 0.;
            double inc = 2.*M_PI / SEGS;

            for( int i = 0; i < SEGS; ++i )
            {
                ret.push_back( getPosition( u, v )*scale );
                u += inc;
            }

            return vector<Vec3>( ret );
        }

		double CrossSectionCircular::unNormalise( double u, double v ) const
		{
			return u * 2. * M_PI;
		}

    }
}
