/**
 * \file
 * \brief Defines fg::gc::MorphCrossSec
 * \author james
 *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#include <list>

#include "fg/gc/morphcrosssec.h"

using namespace std;

namespace fg {
namespace gc {

MorphCrossSec::MorphCrossSec( const vector<PBezInterpDiv > &interps)
{
	for( int i = 0; i < interps.size() - 1; ++i ) {
		addSegment( interps[i], interps[i+1] );
	}
}

Vec3 MorphCrossSec::getPosition( double u, double v ) const
{
	return getCrossSectionInterp( v ).getPosition( u );
}

Vec3 MorphCrossSec::getDerivativeU( double u, double v ) const
{
	return getCrossSectionInterp( v ).getDerivative( u );
}

Vec3 MorphCrossSec::getDerivativeV( double u, double v ) const
{
    return Vec3(0.,0.,0.);
}

vector<Vec3> MorphCrossSec::getCrossSection( double v, double scale ) const
{
	int n = -1;
	Interpolator<Vec3> * it = getCrossSectionInterp(v).scale( scale );
	vector<Vec3> data = it->getApproxVector( n );
	delete it;
	return data;
}

PBezInterpDiv MorphCrossSec::getCrossSectionInterp( double v ) const
{
	int seg = (int) (v);
	seg = clamp<int>(seg, 0, mSourceCP.size() - 1 );

	float t = v - seg;

	vector<Vec3> cp( mSourceCP[seg].size() );
	vector< pair<Vec3,Vec3> > grad( mSourceCP[seg].size() );

	if( fabs( t ) < 1E-03 ) {
		cp = mSourceCP[seg];
		grad = mSourceGrad1[seg];
	}
	else if( fabs( t - 1. ) < 1E-03 ) {
		cp = mAttractorCP[seg];
		grad = mAttractorGrad1[seg];
	}
	else {
		for( int i = 0; i < mSourceCP[seg].size(); ++i ) {
			cp[i] = (mSourceCP[seg])[i] * (1. - t) + (mAttractorCP[seg])[i] * t;
			grad[i].first = (mSourceGrad1[seg])[i].first * (1. - t) + (mAttractorGrad1[seg])[i].first * t;
			grad[i].second = (mSourceGrad1[seg])[i].second * (1. - t) + (mAttractorGrad1[seg])[i].second * t;
		}
	}

	PBezInterpDiv cs( cp, grad );
	cs.setOpen( false );
	return cs;
}

void MorphCrossSec::addSegment( const PBezInterpDiv &source, const PBezInterpDiv &attractor )
{
	// The data for our stored curves
	vector<Vec3> aCP, sCP;
	vector< pair<Vec3,Vec3> > aGrad, sGrad;

	// The total lengths
	double sLength = getTotalLength( source );
	double aLength = getTotalLength( attractor );

	// seg lengths, running totals
	double sSegLen = 0., aSegLen = 0.;
	// Previous seg length;
	double sSegLenPre = .0, aSegLenPre = 0.;
	// segment splines
	BezInterp<Vec3> sSeg;
	BezInterp<Vec3> aSeg;

	// Data required to perform 'splits'
	vector< pair< int, double > > sSplits;
	vector< pair< int, double > > aSplits;
	

    // Store the first control point
	aCP.push_back( attractor.getControlPoint( 0 ) );
	aGrad.push_back( attractor.getGradient( 0 ) );

	sCP.push_back( source.getControlPoint( 0 ) );
	sGrad.push_back( source.getGradient( 0 ) );

	int i = 1;
	int j = 1;

	// Get the current segments
	sSeg = source.getSegmentInterpolator( 0 );
	aSeg = attractor.getSegmentInterpolator( 0 );

	// Get the ratio of lengths
	sSegLen += getLength( sSeg, 0., 1. ) / sLength;
	aSegLen += getLength( aSeg, 0., 1. ) / aLength;

    // While control points remain
	while( i < source.getNumControlPoints() || j < attractor.getNumControlPoints() ) {
		
		// Close enough, store as one point
		if( i < source.getNumControlPoints() && j < attractor.getNumControlPoints() && 
			          fabs(sSegLen - aSegLen) < mThreashold ) {
			aCP.push_back( attractor.getControlPoint( j ) );
			aGrad.push_back( attractor.getGradient( j ) );

			sCP.push_back( source.getControlPoint( i ) );
			sGrad.push_back( source.getGradient( i ) );

			sSegLenPre = sSegLen;
			aSegLenPre = aSegLen;

			// Get the current segments
			sSeg = source.getSegmentInterpolator( i );
			aSeg = attractor.getSegmentInterpolator( j );

			// Get the ratio of lengths
			sSegLen += getLength( sSeg, 0., 1. ) / sLength;
			aSegLen += getLength( aSeg, 0., 1. ) / aLength;
			++i;
			++j;
		} else if (sSegLen < aSegLen ) { 
			// attractor seg is longer, add source cp
			sCP.push_back( source.getControlPoint( i ) );
			sGrad.push_back( source.getGradient( i ) );

			double t = getTime( aSeg, (sSegLen - aSegLenPre) * aLength );
			aCP.push_back( aSeg.getPosition( t ) );
			aGrad.push_back( pair<Vec3,Vec3>( aSeg.getDerivative( t ) / 3., aSeg.getDerivative( t ) / 3. ) );

			aSplits.push_back( pair< int, double >( aCP.size() - 1, t ) );

			sSegLenPre = sSegLen;
			sSeg = source.getSegmentInterpolator( i );
			sSegLen += getLength( sSeg, 0., 1. ) / sLength;
			++i;
		} else { 
			// source is longer, add attractor cp
			aCP.push_back( attractor.getControlPoint( j ) );
			aGrad.push_back( attractor.getGradient( j ) );

			double t = getTime( sSeg, (aSegLen - sSegLenPre) * sLength );
			sCP.push_back( sSeg.getPosition( t ) );
			sGrad.push_back( pair<Vec3,Vec3>( sSeg.getDerivative( t ) / 3., sSeg.getDerivative( t ) / 3. ) ) ;

			sSplits.push_back( pair< int, double >( sCP.size() - 1, t ) );

			aSegLenPre = aSegLen;
			aSeg = attractor.getSegmentInterpolator( j );
			aSegLen += getLength( aSeg, 0., 1. ) / aLength;
			++j;
		}
	}

	// Scale the gradients for the splits
	for( i = 0; i < sSplits.size(); ++i ) {
		int index = sSplits[i].first;

		sGrad[index-1].second = sGrad[index-1].second * sSplits[i].second;

		sGrad[(index+1)%sCP.size()].first = sGrad[(index+1)%sCP.size()].first * (1. - sSplits[i].second);

		sGrad[index].first = sGrad[index].first * sSplits[i].second;
		sGrad[index].second = sGrad[index].second * (1. - sSplits[i].second);
	}
	// Scale the gradients for the splits
	for( i = 0; i < aSplits.size(); ++i ) {
		int index = aSplits[i].first;

		aGrad[index-1].second = aGrad[index-1].second * aSplits[i].second;

		aGrad[(index+1)%aCP.size()].first = aGrad[(index+1)%aCP.size()].first * (1. - aSplits[i].second);

		aGrad[index].first = aGrad[index].first * aSplits[i].second;
		aGrad[index].second = aGrad[index].second * (1. - aSplits[i].second);
	}


	// Add the new curves
	mSourceCP.push_back( sCP );
	mSourceGrad1.push_back( sGrad );
	mAttractorCP.push_back( aCP );
	mAttractorGrad1.push_back( aGrad );
}

double MorphCrossSec::getLength( const BezInterp<Vec3> &spline, double t0, double t1 )
{
    return RombergIntegral(8, t0, t1, spline);
}

double MorphCrossSec::getTime( const BezInterp<Vec3> &spline, double length, int iterations, double tolerance )
{
    if (length <= 0.)
    {
        return 0.;
    }

    if (length >= getLength(spline, 0., 1.) )
    {
        return 1.;
    }

    // Initial guess for Newton's method.
    double ratio = length/getLength(spline,0.,1.);
    double oneMinusRatio = 1. - ratio;
    double t = oneMinusRatio*0. + ratio*1.;

    // Initial root-bounding interval for bisection.
    double lower = 0., upper = 1.;

    for (int i = 0; i < iterations; ++i)
    {
        double difference = getLength(spline,0., t) - length;
        if (fabs(difference) < tolerance)
        {
            // |L(t)-length| is close enough to zero, report t as the time
            // at which 'length' is attained.
            return t;
        }

        // Generate a candidate for Newton's method.
        double tCandidate = t - difference/getSpeed( t, spline );

        // Update the root-bounding interval and test for containment of the
        // candidate.
        if (difference > 0.)
        {
            upper = t;
            if (tCandidate <= lower)
            {
                // Candidate is outside the root-bounding interval.  Use
                // bisection instead.
                t = 0.5*(upper + lower);
            }
            else
            {
                // There is no need to compare to 'upper' because the tangent
                // line has positive slope, guaranteeing that the t-axis
                // intercept is smaller than 'upper'.
                t = tCandidate;
            }
        }
        else
        {
            lower = t;
            if (tCandidate >= upper)
            {
                // Candidate is outside the root-bounding interval.  Use
                // bisection instead.
                t = (0.5)*(upper + lower);
            }
            else
            {
                // There is no need to compare to 'lower' because the tangent
                // line has positive slope, guaranteeing that the t-axis
                // intercept is larger than 'lower'.
                t = tCandidate;
            }
        }
    }

    // A root was not found according to the specified number of iterations
    // and tolerance.  You might want to increase iterations or tolerance or
    // integration accuracy.  However, in this application it is likely that
    // the time values are oscillating, due to the limited numerical
    // precision of 32-bit floats.  It is safe to use the last computed time.
    return t;
}

double MorphCrossSec::getSpeed( double t, const BezInterp<Vec3> &spline )
{
    return (spline.getDerivative( t )).length();
}

double MorphCrossSec::RombergIntegral( int order, double a, double b,
                                       const BezInterp<Vec3> &data  )
{
    double** rom = Interpolator<double>::allocate2dArray( order, 2 );

    double h = b - a;

    rom[0][0] = (0.5)*h*(getSpeed(a, data) + getSpeed(b, data));
    for (int i0 = 2, p0 = 1; i0 <= order; ++i0, p0 *= 2, h *= 0.5)
    {
        // Approximations via the trapezoid rule.
        double sum = 0.;
        int i1;
        for (i1 = 1; i1 <= p0; ++i1)
        {
            sum += getSpeed(a + h*(i1-0.5), data);
        }

        // Richardson extrapolation.
        rom[1][0] = (0.5)*(rom[0][0] + h*sum);
        for (int i2 = 1, p2 = 4; i2 < i0; ++i2, p2 *= 4)
        {
            rom[1][i2] = (p2*rom[1][i2-1] - rom[0][i2-1])/(p2-1);
        }

        for (i1 = 0; i1 < i0; ++i1)
        {
            rom[0][i1] = rom[1][i1];
        }
    }

    double result = rom[0][order-1];
    Interpolator<double>::delete2dArray( rom );
    return result;
}

double MorphCrossSec::getTotalLength( const PBezInterpDiv &spline )
{
	double length = 0.;
	for( int i = 0; i < spline.getNumSegments(); ++i) {
		length += getLength( spline.getSegmentInterpolator( i ), 0., 1. );
	}
	return length;
}

PBezInterpDiv MorphCrossSec::addPoint( const PBezInterpDiv &spline, int seg, float localT )
{
	vector<Vec3> cp = spline.getControlPoints();
	vector< pair<Vec3,Vec3> > grad = spline.getGradients();
	BezInterp<Vec3> segInterp = spline.getSegmentInterpolator( seg );

	Vec3 pos = segInterp.getPosition( localT );
	Vec3 der = segInterp.getDerivative( localT );

	cp.insert( cp.begin() + seg, pos );
	grad.insert( grad.begin() + seg, pair<Vec3, Vec3>(der, der) );

	return PBezInterpDiv( cp, grad );
}

}
}
