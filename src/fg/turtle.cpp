#include "fg/turtle.h"

#include <iostream>

namespace fg {
    namespace gc {
        Turtle::Turtle()
        {
            mState.frame.SetIdentity();
			mState.carrierMode = BEZIER_CARRIER;
			mState.crossSectionMode = SCALED_INTERPOLATOR_CROSS_SECTION;
			mState.currentCS = 0;
			mState.scale = 1.;
			mState.stiffness = std::pair< double, double >(0.6, 0.6);

			std::vector<Vec3> arr;
			std::vector< std::pair<Vec3, Vec3> > grad;
    		arr.push_back( Vec3(.1, 0., 0.) );
		    arr.push_back( Vec3(0., .1, 0.) );
		    arr.push_back( Vec3(-.1, 0., 0.) );
		    arr.push_back( Vec3(0., -.1, 0.) );
		
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.05,0.),Vec3(0.,0.05,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(-0.05,0.,0.),Vec3(-0.05,0.,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,-0.05,0.),Vec3(0.,-0.05,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.05,0.,0.),Vec3(0.05,0.,0.)));

			mCrossSecLibrary.push_back( new spline::PBezInterpDiv( arr, grad ) );
			mCrossSecLibrary.back()->setOpen(false);
        }

        Turtle::~Turtle()
        {
            while( !mScalers.empty() ) {
                delete mScalers.front();
                mScalers.erase( mScalers.begin() );
            }

            while( !mCrossSecLibrary.empty() ) {
                delete mCrossSecLibrary.front();
                mCrossSecLibrary.erase( mCrossSecLibrary.begin() );
            }

            while( !mCarriers.empty() ) {
                delete mCarriers.front();
                mCarriers.erase( mCarriers.begin() );
            }

            while( !mCrossSections.empty() ) {
                delete mCrossSections.front();
                mCrossSections.erase( mCrossSections.begin() );
            }

            while( !mCylinders.empty() ) {
                delete mCylinders.front();
                mCylinders.erase( mCylinders.begin() );
            }
        }

        void Turtle::setFrame( Vec3 pos, Vec3 heading, Vec3 up )
		{
			Mat4 tmp1, tmp2;
			tmp1.setTranslate( pos.getX(), pos.getY(), pos.getZ() );
			tmp2.set( up.cross(heading), up, heading );
			mState.frame = tmp1 * tmp2;
		}

        void Turtle::move( double distance )
        {
            Mat4 trans;
            trans.SetTranslate( 0., 0., distance );
            mState.frame = mState.frame * trans;
        }

        void Turtle::yaw( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( -theta, Vec3( 0., 1., 0. ) );
            mState.frame = mState.frame * rot;
        }

        void Turtle::pitch( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( theta, Vec3( 1., 0., 0. ) );
            mState.frame = mState.frame * rot;
        }

        void Turtle::roll( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( theta, Vec3( 0., 0., 1. ) );
            mState.frame = mState.frame * rot;
        }

        void Turtle::beginCylinder()
        {
			mScaleArr.clear();
            mPrevFrames.clear();
			mDomains.clear();
			mStiffnessArr.clear();

			switch(mState.crossSectionMode)
			{
				case SCALED_INTERPOLATOR_CROSS_SECTION:
				{
					mDomains.push_back( std::pair<double, double>( 0., 0. ) );
					mScaleArr.push_back( mState.scale > 0. ? mState.scale : 1. );
					mState.scale = -1.;
					break;
				}
				default:
				{
					break;
				}
			}

            mPrevFrames.push_back( mState.frame );
			mStiffnessArr.push_back( mState.stiffness );
        }

        void Turtle::addPoint( )
        {
			switch(mState.crossSectionMode)
			{
				case SCALED_INTERPOLATOR_CROSS_SECTION:
				{
					if (mState.scale > 0.)
					{
						mDomains.push_back( std::pair<double,double>( mPrevFrames.size(), mScaleArr.size() ) );
						mScaleArr.push_back( mState.scale );
						mState.scale = -1.;
					}
					break;
				}
				default:
				{
					break;
				}
			}
            mPrevFrames.push_back( mState.frame );
			mStiffnessArr.push_back( mState.stiffness );
        }

        void Turtle::endCylinder()
        {
			switch(mState.crossSectionMode)
			{
				case SCALED_INTERPOLATOR_CROSS_SECTION:
				{
					if (mState.scale > 0.)
					{
						mDomains.push_back( std::pair<double,double>( mPrevFrames.size(), mScaleArr.size() ) );
						mScaleArr.push_back( mState.scale );
						mState.scale = -1.;
					}
					if (mScaleArr.size() == 1)
					{
						mDomains.push_back( std::pair<double, double>( (double) mPrevFrames.size(), (double) mScaleArr.size() ) );
						mScaleArr.push_back( mScaleArr.back() );
					}

					mScalers.push_back( new spline::PBezInterp<double>( mScaleArr ) );
					mCrossSections.push_back( new ScaleInterpCrossSec( *( mCrossSecLibrary[mState.currentCS] ), *( mScalers.back() ) ) );
					break;
				}
				case INTERPOLATOR_CROSS_SECTION:
				{
					mCrossSections.push_back( new InterpCrossSec( *( mCrossSecLibrary[mState.currentCS] ) ) );
				}
				default:
				{
            		mCrossSections.push_back( new CrossSectionCircular( 0.1 ) );
					break;
				}
			}

            mPrevFrames.push_back( mState.frame );
			mStiffnessArr.push_back( mState.stiffness );
			switch(mState.carrierMode)
			{
				case BEZIER_CARRIER:
            		mCarriers.push_back( new PBezCarrier( mPrevFrames, mStiffnessArr ) );
					break;
				case LINEAR_CARRIER:
				default:
            		mCarriers.push_back( new LinCarrier( mPrevFrames ) );
					break;
			}

            mCylinders.push_back( new GeneralisedCylinder( *( mCarriers.back() ), *( mCrossSections.back() ), mPrevFrames, mDomains ) );
        }

		void Turtle::setStiffness(double s1, double s2)
		{
			mState.stiffness = std::pair<double,double> (s1,s2);
		}

		void Turtle::setScale(double scale)
		{
			mState.scale = scale;
		}

		void Turtle::setCrossSection(int index)
		{
			mState.currentCS = clamp<int>( index, 0, mCrossSecLibrary.size() - 1 );
		}

		void Turtle::pushState()
		{
			mStateStack.push( mState );
		}

		void Turtle::popState()
		{
			mState = mStateStack.top();
			mStateStack.pop();
		}

		void Turtle::setCarrierMode( int carrierMode )
		{
			mState.carrierMode = carrierMode;
		}

		void Turtle::setCrossSectionMode( int crossSectionMode )
		{
			mState.crossSectionMode = crossSectionMode;
		}

		void Turtle::beginCrossSection()
		{
			mPrevFrames.clear();
			mStiffnessArr.clear();

			mPrevFrames.push_back( mState.frame );
			mStiffnessArr.push_back( mState.stiffness );
		}

		int Turtle::endCrossSection()
		{
			mPrevFrames.push_back( mState.frame );
			mStiffnessArr.push_back( mState.stiffness );

			std::vector<Vec3> pos;
			std::vector<std::pair<Vec3,Vec3> > grad;

			PBezCarrier::stiffnessToGrad( mPrevFrames, mStiffnessArr, pos, grad );

			mCrossSecLibrary.push_back( new spline::PBezInterpDiv(pos, grad) );
			mCrossSecLibrary.back()->setOpen( false );

			//std::cout << mCrossSecLibrary.back()->isOpen( ) << std::endl;

			return mCrossSecLibrary.size() - 1;
		}

        boost::shared_ptr< Mesh > Turtle::getMesh( int n, int m )
        {
			Mesh::MeshBuilder mb;
			for( int i = 0; i < mCylinders.size(); ++i)
			{
            	mCylinders[i]->createMesh( mb, n, m );
			}
			return mb.createMesh();
        }
    }
}
