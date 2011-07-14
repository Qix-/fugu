namespace fg {
	namespace spline {

template < class T >
LinearInterpolator < T >::LinearInterpolator(const std::vector<T> &mControlPoints)
{
    Interpolator < T >::setControlPoints(mControlPoints);
}

template < class T >
LinearInterpolator < T >::LinearInterpolator()
{
}

template<class T>
LinearInterpolator<T>::LinearInterpolator(const LinearInterpolator<T> &other)
{
	*this = other;
}

template<class T>
LinearInterpolator<T>& LinearInterpolator<T>::operator=(const LinearInterpolator<T> &other)
{
	Interpolator<T>::operator=(other);
	return *this;
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

    for (int i = 0; i <= n; ++i) {
        data[i] = Interpolator<T>::getControlPoint(i);
    }

    return data;
}

template < class T >
std::vector<T> LinearInterpolator < T >::getApproxVector(int &n) const
{
	if (n < 1 || n > Interpolator<T>::getNumControlPoints() - 1)
		n = Interpolator<T>::getNumControlPoints() - 1;

    return Interpolator<T>::getControlPoints();
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

    return clamp(externalT, min, max);
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
