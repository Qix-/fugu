#include "fg/gc/turtle.h"

#include <iostream>

namespace fg {
    namespace gc {
        Turtle::Turtle()
        {
            mState.frame.SetIdentity();
			mState.carrierMode = BEZIER_CARRIER;
			mState.crossSectionMode = MORPH_CROSS_SECTION;
			mState.currentCS = 0;
			mState.scale = 1.;
			mState.stiffness = std::pair< double, double >(0.6, 0.6);

			std::vector<Vec3> arr;
			std::vector< std::pair<Vec3, Vec3> > grad;
    		arr.push_back( Vec3(1., 0., 0.) );
		    arr.push_back( Vec3(0., 1., 0.) );
		    arr.push_back( Vec3(-1., 0., 0.) );
		    arr.push_back( Vec3(0., -1., 0.) );
		
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.5,0.),Vec3(0.,0.5,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(-0.5,0.,0.),Vec3(-0.5,0.,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,-0.5,0.),Vec3(0.,-0.5,0.)));
			grad.push_back( std::pair<Vec3,Vec3> ( Vec3(0.5,0.,0.),Vec3(0.5,0.,0.)));

			mCrossSecLibrary.push_back( new PBezInterpDiv( arr, grad ) );
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

		void Turtle::setFrameRel( Vec3 pos, Vec3 heading, Vec3 up)
		{
			Mat4 tmp1, tmp2;
			tmp1.setTranslate( pos.getX(), pos.getY(), pos.getZ() );
			tmp2.set( up.cross(heading), up, heading );

			mState.frame = mState.frame * tmp1 * tmp2;
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
			if( mState.currentCS == -1 )
				mState.currentCS = mState.crossSecArr.back();

			if( mState.scale < 0. )
				mState.scale = mState.scaleArr.back();

			mState.scaleArr.clear();
            mState.prevFrames.clear();
			mState.scaleDomains.clear();
			mState.stiffnessArr.clear();
			mState.crossSecDomains.clear();
			mState.crossSecArr.clear();
			mState.strips.clear();

			mState.scaleDomains.push_back( std::pair<double, double>( 0., 0. ) );
			mState.scaleArr.push_back( mState.scale > 0. ? mState.scale : 1. );
			mState.scale = -1.;

			switch(mState.crossSectionMode)
			{
				case MORPH_CROSS_SECTION:
				{
					mState.crossSecDomains.push_back( std::pair<double, double>( 0., 0. ) );
					mState.crossSecArr.push_back( mState.currentCS );
					mState.currentCS = -1;
					break;
				}
				default:
				{
					break;
				}
			}

            mState.prevFrames.push_back( mState.frame );
			mState.stiffnessArr.push_back( mState.stiffness );
        }

        void Turtle::addPoint( )
		{
			addPoint( 5 );
		}

        void Turtle::addPoint( int n )
        {
			mState.strips.push_back( n );

			if (mState.scale > 0.)
			{
				mState.scaleDomains.push_back( std::pair<double,double>( mState.prevFrames.size(), mState.scaleArr.size() ) );
				mState.scaleArr.push_back( mState.scale );
				mState.scale = -1.;
			}

			switch(mState.crossSectionMode)
			{
				case MORPH_CROSS_SECTION:
				{
					if (mState.currentCS > -1 )
					{
						mState.crossSecDomains.push_back( std::pair<double, double>( mState.prevFrames.size(), mState.crossSecArr.size() ) );
						mState.crossSecArr.push_back( mState.currentCS );
						mState.currentCS = -1;
					}
					break;
				}
				default:
				{
					break;
				}
			}
            mState.prevFrames.push_back( mState.frame );
			mState.stiffnessArr.push_back( mState.stiffness );
        }

        void Turtle::endCylinder( )
		{
			endCylinder( 5 );
		}

        void Turtle::endCylinder( int n )
        {
			mState.strips.push_back( n );

			if (mState.scale > 0.)
			{
				mState.scaleDomains.push_back( std::pair<double,double>( mState.prevFrames.size(), mState.scaleArr.size() ) );
				mState.scaleArr.push_back( mState.scale );
			}
			if (mState.scaleArr.size() == 1)
			{
				mState.scaleDomains.push_back( std::pair<double, double>( (double) mState.prevFrames.size(), (double) mState.scaleArr.size() ) );
				mState.scaleArr.push_back( mState.scaleArr.back() );
				mState.scale = mState.scaleArr.back();
			}

			mScalers.push_back( new CRInterp<double>( mState.scaleArr ) );


			switch(mState.crossSectionMode)
			{
				case MORPH_CROSS_SECTION:
				{
					if (mState.currentCS > -1 )
					{
						mState.crossSecDomains.push_back( std::pair<double, double>( mState.prevFrames.size(), mState.crossSecArr.size() ) );
						mState.crossSecArr.push_back( mState.currentCS );
					}
					else
					{
						mState.crossSecDomains.push_back( std::pair<double, double>( (double) mState.prevFrames.size(), (double) mState.crossSecArr.size() ) );
						mState.crossSecArr.push_back( mState.crossSecArr.back() );
						mState.currentCS = mState.crossSecArr.back();
					}

					std::vector< PBezInterpDiv > tmp;
					for( int i = 0; i < mState.crossSecArr.size(); ++i )
					{
						tmp.push_back( *mCrossSecLibrary[mState.crossSecArr[i]] );
					}
					mCrossSections.push_back( new MorphCrossSec( tmp ) );
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

//			std::cout << "\n\nCS Domain\n";
//			for( int i = 0; i < mState.crossSecDomains.size(); ++i )
//			{
//				std::cout << mState.crossSecDomains[i].first << ", " << mState.crossSecDomains[i].second << std::endl;
//			}
//			std::cout << "Scale Domain\n";
//			for( int i = 0; i < mState.scaleDomains.size(); ++i )
//			{
//				std::cout << mState.scaleDomains[i].first << ", " << mState.scaleDomains[i].second << std::endl;
//			}

            mState.prevFrames.push_back( mState.frame );
			mState.stiffnessArr.push_back( mState.stiffness );
			switch(mState.carrierMode)
			{
				case BEZIER_CARRIER:
            		mCarriers.push_back( new PBezCarrier( mState.prevFrames, mState.stiffnessArr ) );
					break;
				case LINEAR_CARRIER:
				default:
            		mCarriers.push_back( new LinCarrier( mState.prevFrames ) );
					break;
			}

            mCylinders.push_back( new GeneralisedCylinder( *( mCarriers.back() ), mState.prevFrames, *( mCrossSections.back() ), mState.crossSecDomains, 
			                      *( mScalers.back() ), mState.scaleDomains, mState.strips ) );
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
			mState.prevFrames.clear();
			mState.stiffnessArr.clear();

			mState.prevFrames.push_back( mState.frame );
			mState.stiffnessArr.push_back( mState.stiffness );
		}

		int Turtle::endCrossSection()
		{
			mState.prevFrames.push_back( mState.frame );
			mState.stiffnessArr.push_back( mState.stiffness );

			std::vector<Vec3> pos;
			std::vector<std::pair<Vec3,Vec3> > grad;

			PBezCarrier::stiffnessToGrad( mState.prevFrames, mState.stiffnessArr, pos, grad );

			mCrossSecLibrary.push_back( new PBezInterpDiv(pos, grad) );
			mCrossSecLibrary.back()->setOpen( false );

			return mCrossSecLibrary.size() - 1;
		}

        Vec3 Turtle::getPosition()
		{
			return mState.frame * Vec3(0.,0.,0.);
		}

        boost::shared_ptr< Mesh > Turtle::getMesh( )
        {
			Mesh::MeshBuilder mb;

			int cap;
			mEndCaps.clear();

			for( int i = 0; i < mCylinders.size(); ++i)
			{
            	cap = mCylinders[i]->createMesh( mb );
				mEndCaps.push_back(cap);
			}
			return mb.createMesh();
        }
    }
}
