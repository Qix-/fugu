#include "fg/gc/pbezinterpdiv.h"
#include "fg/gc/recsubdiv.h"

namespace fg {
	namespace gc {
		std::vector<Vec3> PBezInterpDiv::getApproxVector( int &n ) const
		{
			std::vector<Vec3> data;
			std::vector<Vec3> tmp;
		
		    RecSubDiv sd;
			for (int i = 0; i < PBezInterp<Vec3>::getNumSegments(); ++i)
			{
				sd.approximation_scale(10.);
				sd.angle_tolerance(0.05);
				sd.init( PBezInterp<Vec3>::getSegmentControlPoints(i) );;
				tmp = sd.get_data();
				data.insert(data.end(), tmp.begin(), tmp.end());
				data.pop_back();
			}
		
			//if( Interpolator<Vec3>::mOpen )
			//	data.push_back( data.back() );
		
			return data;
		}

		Interpolator<Vec3> * PBezInterpDiv::scale( double s ) const
		{
			std::vector<Vec3> cp = getControlPoints();
			std::vector< std::pair<Vec3,Vec3> > grad = getGradients();

			for(int i = 0; i < cp.size(); ++i)
			{
				cp[i] = cp[i] * s;
				grad[i].first = grad[i].first * s;
				grad[i].second = grad[i].second * s;
			}

			Interpolator<Vec3> *it = new PBezInterpDiv(cp, grad);
			it->setOpen( Interpolator<Vec3>::isOpen() );
			return it;
		}
	}
}
