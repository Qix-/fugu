namespace fg {
namespace spline {

template<class T>
Interpolator<T>::Interpolator( )
:mControlPoints(NULL)
{}

template<class T>
Interpolator<T>::~Interpolator( )
{
    deleteData();
}

template<class T>
const T *Interpolator<T>::getControlPoints() const
{
    return mControlPoints;
}

template < class T > int Interpolator < T >::getNumControlPoints() const
{
    return mNumControlPoints;
}

template < class T >
void Interpolator < T >::setControlPoints(int numControlPoints, const T *newControlPoints)
{
    if (numControlPoints < 1)
        return;

    deleteData();
    mControlPoints = new T[numControlPoints];

    for (int i = 0; i < numControlPoints; ++i)
    {
        mControlPoints[i] = newControlPoints[i];
    }
    mNumControlPoints = numControlPoints;
}

template < class T >
const T & Interpolator<T>::getControlPoint( int i ) const
{
    clamp( i, 0, getNumControlPoints() );

    return mControlPoints[i];
}

template< class T >
void Interpolator<T>::setControlPoint(int index, const T &cp)
{
    if( index < 0 || index >= getNumControlPoints())
        return;

    mControlPoints[index] = cp;
}

template< class T >
void Interpolator<T>::deleteData( )
{
    if(mControlPoints)
        delete[] mControlPoints;

    mNumControlPoints = 0;
    mControlPoints = NULL;
}

template< class T >
T* Interpolator<T>::getApprox( int &n ) const
{
    if (n < 1)
        n = getNumControlPoints() * 5;

    T *data = new T[n+1];
    double t = 0.f;
    double min, max;
    getDomain(min, max);
    double inc = max / (double) (n);

    for (int i = 0; i <= n; ++i) {
        data[i] = getPosition( t );
        t += inc;
    }

    return data;
}

template< class T >
void Interpolator<T>::get( double t, T *pos, T *der1, T *der2)
{
    if (pos)
    {
        *pos = getPosition(t);
    }
    if (der1)
    {
        *der1 = getDerivative(t);
    }
    if (der2)
    {
        *der2 = getSecondDerivative(t);
    }
}

template< class T >
double Interpolator<T>::clamp(double num, double min, double range)
{
    num = AlmostEqual2sComplement(num, min)
          || num < min ? min : num;
    num = AlmostEqual2sComplement(num, min + range)
          || num > min + range ? min + range : num;
    return num;
}

template< class T >
int Interpolator<T>::clamp(int num, int min, int range)
{
    num = num < min ? min : num;
    num = num > min + range ? min + range : num;
    return num;
}

template< class T >
bool Interpolator<T>::AlmostEqual2sComplement(double A, double B, int maxUlps)
{
    // Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    if (maxUlps < 0 && maxUlps > 4 * 1024 * 1024)
        maxUlps = 1;
    int aInt = *(int *) &A;
    // Make aInt lexicographically ordered as a twos-complement int
    if (aInt < 0)
        aInt = 0x80000000 - aInt;
    // Make bInt lexicographically ordered as a twos-complement int
    int bInt = *(int *) &B;
    if (bInt < 0)
        bInt = 0x80000000 - bInt;
    int intDiff = abs(aInt - bInt);
    if (intDiff <= maxUlps)
        return true;
    return false;
}

template< class T >
double Interpolator<T>::determinant(double a, double b, double c)
{
    return (b * b) - (4 * a * c);
}

/**
 * Assumes a != 0.
 */
template< class T >
int Interpolator<T>::rootsQuad(double a, double b, double det, double *r1, double *r2)
{
    if (AlmostEqual2sComplement(det, 0.f)) {
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

//      cout << "r1 = " << *r1 << ", r2 = " << *r2 << endl;
    return 2;
}

/**
 * Assumes m != 0.
 */
template< class T >
double Interpolator<T>::rootLinear(double m, double c)
{
    return -c / m;
}

template< class T >
int Interpolator<T>::roots(double a, double b, double c, double *r1, double *r2)
{
    // Not a quadratic
    if (AlmostEqual2sComplement(a, 0.f, 1000000000)) {
        // if it not linear
        if (AlmostEqual2sComplement(b, 0.f, 1000000000)) {
            // zero every where
            if (AlmostEqual2sComplement(c, 0.f, 1000000000)) {
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

template <typename T>
T** Interpolator<T>::allocate2dArray (const int bound0, const int bound1)
{
    const int bound01 = bound0*bound1;
    T** data;

    data = new T*[bound1];
    data[0] = new T[bound01];

    // Hook up the pointers to form the 2D array.
    for (int i1 = 1; i1 < bound1; ++i1)
    {
        int j0 = bound0*i1;
        data[i1] = &data[0][j0];
    }

    return data;
}


template <typename T>
void Interpolator<T>::delete2dArray (T **data)
{
    if(data)
    {
        delete[] data[0];
        delete[] data;
        data = 0;
    }
}

}
}
