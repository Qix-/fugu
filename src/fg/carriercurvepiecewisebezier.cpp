#include "fg/fg.h"
#include "fg/mat4.h"
#include "fg/carriercurvepiecewisebezier.h"

namespace fg {
namespace gc {

CarrierCurvePiecewiseBezier::CarrierCurvePiecewiseBezier(int numControlPoints, const Mat4 *refFrames)
{
    mInterpolator = NULL;
    mInflectionPoints = NULL;
    mSegType = NULL;

	mAlphaInt = NULL;
	mBetaInt = NULL;
	mGammaInt = NULL;

	mOrients = NULL;

	std::pair<double,double> stiffness[numControlPoints - 1];

	for (int i = 0; i < numControlPoints - 1; ++i)
	{
		stiffness[i].first = 0.6;
		stiffness[i].second = 0.6;
	}

	setControlPoints(numControlPoints, refFrames, stiffness);
}

CarrierCurvePiecewiseBezier::CarrierCurvePiecewiseBezier(int numControlPoints, const Mat4 *refFrames, const std::pair<double, double> *stiffness)
{
    mInterpolator = NULL;
    mInflectionPoints = NULL;
    mSegType = NULL;

	mAlphaInt = NULL;
	mBetaInt = NULL;
	mGammaInt = NULL;

	mOrients = NULL;

    assert(numControlPoints > 1);
    
	setControlPoints(numControlPoints, refFrames, stiffness);
}

void CarrierCurvePiecewiseBezier::setControlPoints(int numControlPoints, const Mat4 *refFrames, const std::pair<double,double> *stiffness)
{
	deleteData();

    mSegType = new int[numControlPoints - 1];
	mInflectionPoints = new std::pair<double,double>[numControlPoints - 1];
	mOrients = new Mat4[numControlPoints];

	Vec3 tmpCp[numControlPoints];
	std::pair<Vec3, Vec3> tmpGrad[numControlPoints];

    for (int i = 0; i < numControlPoints - 1; ++i)
	{
	    // Get the control points and headings PUT THIS INTO MAT4!!
	    Vec3 p1 = refFrames[i]   * Vec3(0.,0.,0.); // The pos
	    Vec3 p2 = refFrames[i]   * Vec3(0.,0.,1.); // The head
	    Vec3 p4 = refFrames[i+1] * Vec3(0.,0.,0.);
	    Vec3 p3 = refFrames[i+1] * Vec3(0.,0.,1.);
	
	    // Calculate the tangents using coefficients
	    float mag = (p1 - p4).Norm();
	    p2 = p2 * mag * (stiffness[i].first);
	    p3 = p3 * mag * (stiffness[i].second);

        tmpCp[i] = p1;
		tmpCp[i+1] = p4;
		tmpGrad[i].first = p2;
		tmpGrad[i].second = p3;
	}

	mInterpolator = new spline::PiecewiseBezierInterpolator<Vec3>(numControlPoints, tmpCp, tmpGrad);

    for (int i = 0; i < numControlPoints - 1; ++i)
	{
		updateInflectionPoints(i);
	}
}

CarrierCurvePiecewiseBezier::~CarrierCurvePiecewiseBezier()
{
    deleteData();
}

void CarrierCurvePiecewiseBezier::deleteData()
{
    if(mInterpolator)
        delete mInterpolator;

    if(mInflectionPoints)
        delete[] mInflectionPoints;

    if(mSegType)
        delete[] mSegType;

    if(mAlphaInt)
		delete mAlphaInt;
    if(mBetaInt)
		delete mBetaInt;
    if(mGammaInt)
		delete mGammaInt;
	if(mOrients)
		delete mOrients;
}

void CarrierCurvePiecewiseBezier::updateInflectionPoints( int seg )
{
    // No such segment
    if (seg > (signed int) (mInterpolator->getNumSegments()) || seg < 0) {
        return;
    }

    // Check if the line is straight
    const Vec3 *cp = mInterpolator->getSegmentControlPoints(seg);
    if (collinear( cp )) {
        mSegType[seg] = 3;
    } else {
        double ip1, ip2;
        mSegType[seg] = findInflectionPointsVec( cp, &ip1, &ip2 );
        mInflectionPoints[seg] = std::pair<float, float>( ip1, ip2 );
    }
}

const spline::PiecewiseBezierInterpolator<Vec3> * CarrierCurvePiecewiseBezier::getInterpolator() const
{
    return mInterpolator;
}

void CarrierCurvePiecewiseBezier::getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const
{
    getFrenetFrame(t, H, U, L);
}

void CarrierCurvePiecewiseBezier::getFrenetFrame(double t, Vec3 *H, Vec3 *U, Vec3 *L) const
{
    Vec3 vel = mInterpolator->getDerivative( t );
    Vec3 tangent = vel;
    tangent.normalise();

    if (H) {
        *H = tangent;
        if(!(U && L))
            return;
    }

    Vec3 acc = mInterpolator->getSecondDerivative( t );

    double vDotV = vel.dot( vel );
    double vDotA = vel.dot( acc );
    Vec3 norm = acc * vDotV - vel * vDotA;
    norm.normalise();

//    Mat4 rot;
//    rot.SetRotateRad(getQuasiNormalTheta(t),tangent);
//    norm = rot * norm;

    if (U)
    {
        *U = norm;
    }

    if (L)
    {
        *L = norm^tangent;
    }
}

Vec3 CarrierCurvePiecewiseBezier::orient(double v, double x, double y) const
{
    Vec3 U, B;
    getOrientation(v, 0, &U, &B);
    return U*x + B*y;
}

Vec3 CarrierCurvePiecewiseBezier::dOrientDv(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

Vec3 CarrierCurvePiecewiseBezier::dOrientDx(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

Vec3 CarrierCurvePiecewiseBezier::dOrientDy(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

int CarrierCurvePiecewiseBezier::findInflectionPointsVec( const Vec3 *cp, double *i1, double *i2 )
{
    Vec3 a = cp[1] - cp[0];
    Vec3 b = cp[2] - cp[1] - a;
    Vec3 c = cp[3] - cp[2] - a - b*2;

    // Check for inflection points
    int n = roots( c^b, c^a, b^a, i1, i2 );

    // Check they fall in the correct domain
    if (n == 2 && (*i2 < 0 || *i2 > 1))
        n = 1;
    if (*i1 < 0 || *i1 > 1)
    {
        *i1 = *i2;
        --n;
    }
    return n;
}

int CarrierCurvePiecewiseBezier::roots( Vec3 a, Vec3 b, Vec3 c, double *r1, double *r2 )
{
    bool xall, yall, zall;

    // first component
    double rootsx[2];
    int irootsx = roots( a.getX(), b.getX(), c.getX(), &rootsx[0], &rootsx[1] );
    if (irootsx == 0) {
        return 0;
    }
    xall = irootsx < 0;

    // second component
    double rootsy[2];
    int irootsy = roots( a.getY(), b.getY(), c.getY(), &rootsy[0], &rootsy[1] );
    if (irootsy == 0) {
        return 0;
    }
    yall = irootsy < 0;

    // third component
    double rootsz[2];
    int irootsz = roots( a.getZ(), b.getZ(), c.getZ(), &rootsz[0], &rootsz[1] );
    if (irootsz == 0) {
        return 0;
    }
    zall = irootsz < 0;

    // Zero everywhere
    if (xall && yall && zall) {
        return -1;
    }

    // find common roots
    // intersect rootsx rootsy
    double xinty[2];
    int ixinty = 0;
    if (xall) { // x components are everywhere zero
        xinty[0] = rootsy[0];
        xinty[1] = rootsy[1];
        ixinty = irootsy;
    } else if (yall) { // y components are everywhere zero
        xinty[0] = rootsx[0];
        xinty[1] = rootsx[1];
        ixinty = irootsx;
    } else { // do the intersection
        for (int i = 0; i < irootsx; ++i) {
            for (int j = 0; j < irootsy; ++j) {
                if (spline::Interpolator<float>::AlmostEqual2sComplement(rootsx[i], rootsy[j], 100000)) {
                    xinty[ixinty] = rootsx[i];
                    ++ixinty;
                }
            }
        }
    }

    if (ixinty == 0) {
        return 0;
    }

    // intersect xinty and z
    double xintyintz[2];
    int ixintyintz = 0;
    if (xall && yall) { // xinty components are everywhere zero
        xintyintz[0] = rootsz[0];
        xintyintz[1] = rootsz[1];
        ixintyintz = irootsz;
    } else if (zall) { // y components are everywhere zero
        xintyintz[0] = xinty[0];
        xintyintz[1] = xinty[1];
        ixintyintz = ixinty;
    } else { // do the intersection
        for (int i = 0; i < ixinty; ++i) {
            for (int j = 0; j < irootsz; ++j) {
                if (spline::Interpolator<float>::AlmostEqual2sComplement(xinty[i], rootsz[j], 100000)) {
                    xintyintz[ixintyintz] = xinty[i];
                    ++ixintyintz;
                }
            }
        }
    }

    *r1 = xintyintz[0];
    *r2 = xintyintz[1];
    return ixintyintz;
}


double CarrierCurvePiecewiseBezier::determinant(double a, double b, double c)
{
    return (b * b) - (4 * a * c);
}

/**
 * Assumes a != 0.
 */
int CarrierCurvePiecewiseBezier::rootsQuad(double a, double b, double det, double *r1, double *r2)
{
    if (spline::Interpolator<double>::AlmostEqual2sComplement(det, 0.f, 10000000)) {
        *r1 = (-b / (2.f * a));
        *r2 = *r1;
        return 1;
    }

    if (det < 0.f) {
        return 0;
    }

    if (a >= 0.f) {
        *r1 = (-b - sqrt(det)) / (2.f * a);
        *r2 = (-b + sqrt(det)) / (2.f * a);
    } else {
        *r1 = (-b + sqrt(det)) / (2.f * a);
        *r2 = (-b - sqrt(det)) / (2.f * a);
    }

    return 2;
}

/**
 * Assumes m != 0.
 */
double CarrierCurvePiecewiseBezier::rootLinear(double m, double c)
{
    return -c / m;
}

int CarrierCurvePiecewiseBezier::roots(double a, double b, double c, double *r1, double *r2)
{
    // Not a quadratic
    if (spline::Interpolator<double>::AlmostEqual2sComplement(a, 0.f, 10000000)) {
        // if it not linear
        if (spline::Interpolator<double>::AlmostEqual2sComplement(b, 0.f, 10000000)) {
            // zero every where
            if (spline::Interpolator<double>::AlmostEqual2sComplement(c, 0.f, 10000000)) {
                return -1;
            } else {		// Zero nowhere
                return 0;
            }
        } else {		// Solve the linear eq
            *r1 = rootLinear(b, c);
            *r2 = *r1;
            return 1;
        }
    } else {			// Solve the quadratic
        double det = determinant(a, b, c);
        return rootsQuad(a, b, det, r1, r2);
    }
}

bool CarrierCurvePiecewiseBezier::collinear( const Vec3 *p )
{
    Vec3 v1 = p[3] - p[1];
    Vec3 v2 = p[2] - p[1];
    Vec3 v3 = p[3] - p[1];

    return parallel( v1, v2 ) && parallel( v1, v3 ) && parallel( v2, v3 );
}

bool CarrierCurvePiecewiseBezier::parallel( const Vec3 &v1, const Vec3 &v2 )
{
    float l12 = v1.SquaredNorm( );
    float l22 = v2.SquaredNorm( );


    if (spline::Interpolator<double>::AlmostEqual2sComplement( l12, 0.f, 1000 ) || spline::Interpolator<double>::AlmostEqual2sComplement( l22, 0.f, 1000 )) {
        return true;
    }

    float ctheta = v1.dot( v2 )/(sqrt( l12 )*sqrt( l22 ));
    return spline::Interpolator<double>::AlmostEqual2sComplement( ctheta, 1.f, 1000 );
}

}
}
