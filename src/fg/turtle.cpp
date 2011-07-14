#include "fg/turtle.h"

#include <iostream>

namespace fg {
    namespace gc {
        Turtle::Turtle()
        {
            mFrame.SetIdentity();
        }

        Turtle::~Turtle()
        {
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

        void Turtle::move( double distance )
        {
            Mat4 trans;
            trans.SetTranslate( 0., 0., distance );
            mFrame = mFrame * trans;
        }

        void Turtle::yaw( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( theta, Vec3( 0., 1., 0. ) );
            mFrame = mFrame * rot;
        }

        void Turtle::pitch( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( theta, Vec3( 1., 0., 0. ) );
            mFrame = mFrame * rot;
        }

        void Turtle::roll( double theta )
        {
            Mat4 rot;
            rot.SetRotateRad( theta, Vec3( 0., 0., 1. ) );
            mFrame = mFrame * rot;
        }

        void Turtle::beginCylinder()
        {
            mPrevFrames.clear();
            mPrevFrames.push_back( mFrame );
        }

        void Turtle::addPoint()
        {
            mPrevFrames.push_back( mFrame );
        }

        void Turtle::endCylinder()
        {
            mPrevFrames.push_back( mFrame );
            mCarriers.push_back( new PBezCarrier( mPrevFrames ) );
            mCrossSections.push_back( new CrossSectionCircular( 0.1 ) );
            mCylinders.push_back( new GeneralisedCylinder( *( mCarriers.back() ), *( mCrossSections.back() ), mPrevFrames ) );
        }

        boost::shared_ptr< Mesh > Turtle::getMesh( int i, int n, int m )
        {
            return mCylinders[i]->createMesh( n, m );
        }
    }
}