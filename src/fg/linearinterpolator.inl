namespace fg {
	namespace spline {

template < class T >
LinearInterpolator < T >::LinearInterpolator(int numControlPoints, const T *mControlPoints)
{
    Interpolator < T >::setControlPoints(numControlPoints, mControlPoints);
}

template < class T >
T LinearInterpolator < T >::getPosition(double t) const
{
    double internalT = getInternalT(t);
    int cp = getControlPointIndex(internalT);
    internalT -= (double)cp;

    return Interpolator<T>::mControlPoints[cp]*(1.f - internalT) + Interpolator<T>::mControlPoints[cp + 1]*internalT;
}

template < class T >
T LinearInterpolator < T >::getDerivative(double t) const
{
    int cp = getControlPointIndex(t);

    return (Interpolator<T>::mControlPoints[cp+1] - Interpolator<T>::mControlPoints[cp]);
}

template < class T >
T LinearInterpolator <T>::getSecondDerivative(double t) const
{
    return Interpolator<T>::getControlPoint(0) - Interpolator<T>::getControlPoint(0);
}

template < class T >
T * LinearInterpolator < T >::getApprox(int &n) const
{
    if (n < 1 || n > Interpolator<T>::getNumControlPoints() - 1)
        n = Interpolator<T>::getNumControlPoints() - 1;

    T *data = new T[n+1];
    double t = 0.f;
    double inc = 1.f / (double) (n);

    for (int i = 0; i <= n; ++i) {
        data[i] = getPosition( t );
        t += inc;
    }

    return data;
}

template < class T >
void LinearInterpolator <T>::getInternalDomain(double &min, double &max) const
{
    min = 0.f;
    max = (double) (Interpolator < T >::getNumControlPoints() - 1);
}

template < class T >
int LinearInterpolator < T >::getControlPointIndex(double InternalT) const
{
    int ti = (int) InternalT;
    return ti;
}


template < class T >
double LinearInterpolator<T>::getInternalT(double externalT) const
{
    double min, max;
    getInternalDomain(min, max);

    return Interpolator<double>::clamp(externalT, min, max);
}

template < class T >
void LinearInterpolator < T >::getDomain(double &min, double &max) const
{
	min = 0.f;
	max = (double) Interpolator<T>::getNumControlPoints() - 1;
}

template < class T >
int LinearInterpolator < T >::getNumSegments() const
{
	return Interpolator<T>::getNumControlPoints()-1;
}
	}}
