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
    double t;
    double min, max;
    getDomain(min, max);
	t = min;
    double inc = (max - min) / (double) (n);

    for (int i = 0; i <= n; ++i) {
        data[i] = getPosition( t );
        t += inc;
    }

    return data;
}

template< class T >
void Interpolator<T>::get( double t, T *pos, T *der1, T *der2) const
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

//void Interpolator<Vec3>::getFrenetFrame(double t, Vec3 *T, Vec3 *N, Vec3 *B) const
//{
//    Vec3 vel = getDerivative( t );
//    Vec3 tangent = vel;
//	tangent.normalise();
//
//    if (T){
//		*T = tangent;
//		if(!(N && B))
//			return;
//	}
//    
//    Vec3 acc = getSecondDerivative( t );
//
//    float vDotV = vel.dot( vel );
//    float vDotA = vel.dot( acc );
//    Vec3 norm = acc * vDotV - vel * vDotA;
//    norm.normalise();
//
//	if (N)
//	{
//		*N = norm;
//	}
//
//    if (B)
//	{
//      *B = norm^tangent;
//	}
//}
//
//Vec3 Interpolator<Vec3>::getTangent(double t) const
//{
//	Vec3 T;
//    get(t, &T, 0, 0);
//	return T;
//}
//
//Vec3 Interpolator<Vec3>::getNormal(double t) const
//{
//	Vec3 N;
//    get(t, 0, &N, 0);
//	return N;
//}
//
//Vec3 Interpolator<Vec3>::getBinormal(double t) const
//{
//	Vec3 B;
//    get(t, 0, 0, &B);
//	return B;
//}
//
//const Vec3 *Interpolator<Vec3>::getControlPoints() const
//{
//    return mControlPoints;
//}
//
//int Interpolator < Vec3 >::getNumControlPoints() const
//{
//    return mNumControlPoints;
//}
//
//void Interpolator < Vec3 >::setControlPoints(int numControlPoints, const Vec3 *newControlPoints)
//{
//    if (numControlPoints < 1)
//        return;
//
//    deleteData();
//    mControlPoints = new Vec3[numControlPoints];
//
//    for (int i = 0; i < numControlPoints; ++i)
//    {
//        mControlPoints[i] = newControlPoints[i];
//    }
//    mNumControlPoints = numControlPoints;
//}
//
//const Vec3 & Interpolator<Vec3>::getControlPoint( int i ) const
//{
//    Interpolator<float>::clamp( i, 0, getNumControlPoints() );
//
//    return mControlPoints[i];
//}
//
//void Interpolator<Vec3>::setControlPoint(int index, const Vec3 &cp)
//{
//    if( index < 0 || index >= getNumControlPoints())
//        return;
//
//    mControlPoints[index] = cp;
//}
//
//void Interpolator<Vec3>::deleteData( )
//{
//    if(mControlPoints)
//        delete[] mControlPoints;
//
//    mNumControlPoints = 0;
//    mControlPoints = NULL;
//}
//
//Vec3* Interpolator<Vec3>::getApprox( int &n ) const
//{
//    if (n < 1)
//        n = getNumControlPoints() * 5;
//
//    Vec3 *data = new Vec3[n+1];
//    double t = 0.f;
//    double min, max;
//    getDomain(min, max);
//    double inc = max / (double) (n);
//
//    for (int i = 0; i <= n; ++i) {
//        data[i] = getPosition( t );
//        t += inc;
//    }
//
//    return data;
//}
//
//void Interpolator<Vec3>::get( double t, Vec3 *pos, Vec3 *der1, Vec3 *der2) const
//{
//    if (pos)
//    {
//        *pos = getPosition(t);
//    }
//    if (der1)
//    {
//        *der1 = getDerivative(t);
//    }
//    if (der2)
//    {
//        *der2 = getSecondDerivative(t);
//    }
//}
//
//double Interpolator<Vec3>::clamp(double num, double min, double range)
//{
//    num = AlmostEqual2sComplement(num, min)
//          || num < min ? min : num;
//    num = AlmostEqual2sComplement(num, min + range)
//          || num > min + range ? min + range : num;
//    return num;
//}
//
//int Interpolator<Vec3>::clamp(int num, int min, int range)
//{
//    num = num < min ? min : num;
//    num = num > min + range ? min + range : num;
//    return num;
//}
//
//bool Interpolator<Vec3>::AlmostEqual2sComplement(double A, double B, int maxUlps)
//{
//    // Make sure maxUlps is non-negative and small enough that the
//    // default NAN won't compare as equal to anything.
//    if (maxUlps < 0 && maxUlps > 4 * 1024 * 1024)
//        maxUlps = 1;
//    int aInt = *(int *) &A;
//    // Make aInt lexicographically ordered as a twos-complement int
//    if (aInt < 0)
//        aInt = 0x80000000 - aInt;
//    // Make bInt lexicographically ordered as a twos-complement int
//    int bInt = *(int *) &B;
//    if (bInt < 0)
//        bInt = 0x80000000 - bInt;
//    int intDiff = abs(aInt - bInt);
//    if (intDiff <= maxUlps)
//        return true;
//    return false;
//}

}

}
