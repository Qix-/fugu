namespace fg {
	namespace spline {

template<class T>
PiecewiseBezierInterpolator<T>::PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints)
:Interpolator<T>()
,mSegInterpolators(NULL)
,mGradients(NULL)
{
	setControlPoints(numControlPoints, controlPoints);
}

template<class T>
PiecewiseBezierInterpolator<T>::PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints, const std::pair<T, T> *gradients)
:Interpolator<T>()
,mSegInterpolators(NULL)
,mGradients(NULL)
{
	setControlPoints(numControlPoints, controlPoints, gradients);
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoints(int numControlPoints, const T *controlPoints)
{
	std::pair<T,T> *gradients = new std::pair<T,T>[numControlPoints-1];

    gradients[0].first = (controlPoints[1] - controlPoints[0]) * .5f;
    gradients[numControlPoints - 2].second = (controlPoints[numControlPoints - 1] - controlPoints[numControlPoints - 2]) * .5f;
	std::cout << "g4 = " << gradients[numControlPoints - 2].second << std::endl;
	for(int i = 0; i < numControlPoints - 1; ++i)
		{
			gradients[i].second = (controlPoints[i + 1] - controlPoints[i-1]) * 0.5f + Vec3(1., 1., 1.);
			gradients[i+1].first = gradients[i].first;
		}
	setControlPoints(numControlPoints, controlPoints, gradients);
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoints(int numControlPoints, const T *controlPoints, const std::pair<T, T> *gradients)
{
	deleteData();
    Interpolator<T>::setControlPoints(numControlPoints, controlPoints);

    mGradients = new std::pair<T,T>[numControlPoints-1];
    for (int i = 0; i < numControlPoints-1; ++i)
    {
		mGradients[i].first = gradients[i].first;
		mGradients[i].second = gradients[i].second;
	}

	mSegInterpolators = new BezierInterpolator<T>*[getNumSegments()];
	T cp[4];
    for (int i = 0; i < getNumSegments(); ++i)
    {
		cp[0] = controlPoints[i];
		cp[1] = controlPoints[i] + gradients[i].first;
		cp[2] = controlPoints[i + 1] - gradients[i].second;
		cp[3] = controlPoints[i + 1];
        mSegInterpolators[i] = new BezierInterpolator<T>(3, cp);
    }
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoint(int index, const T &cp, const T &grad)
{
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoint(int index, const T &cp)
{
}

template<class T>
PiecewiseBezierInterpolator<T>::~PiecewiseBezierInterpolator( )
{
    deleteData();
}

template < class T >
void PiecewiseBezierInterpolator < T >::getDomain(double &min, double &max) const
{
	min = 0.f;
	max = (double) getNumSegments();
}

template< class T >
void PiecewiseBezierInterpolator<T>::deleteData( )
{
	if (mSegInterpolators)
	{
		for(int i = 0; i < getNumSegments(); ++i)
		{
			if(mSegInterpolators[i])
				delete mSegInterpolators[i];
		}
		delete[] mSegInterpolators;
	}
	if (mGradients)
		delete[] mGradients;

	mSegInterpolators = NULL;
	mGradients = NULL;

	Interpolator<T>::deleteData( );
}

template< class T >
int PiecewiseBezierInterpolator<T>::getNumSegments() const 
{
	return Interpolator<T>::getNumControlPoints() - 1;
}

template<class T>
T PiecewiseBezierInterpolator<T>::getPosition (double t) const
{
	int seg = getSegment(t);
    return mSegInterpolators[seg]->getPosition( t - (double) seg );
}

template<class T>
T PiecewiseBezierInterpolator<T>::getDerivative (double t) const
{
	int seg = getSegment(t);
    return mSegInterpolators[seg]->getDerivative( t - (double) seg );
}

template<class T>
T PiecewiseBezierInterpolator<T>::getSecondDerivative (double t) const
{
	int seg = getSegment(t);
    return mSegInterpolators[seg]->getSecondDerivative( t - (double) seg );
}

template < class T >
int PiecewiseBezierInterpolator < T >::getSegment(double t) const
{
    int ti = (int) t;
	ti = Interpolator<T>::clamp(ti, 0, getNumSegments() - 1);
    return ti;
}

template <class T>
const T* PiecewiseBezierInterpolator<T>::getSegmentControlPoints(int seg) const
{
	seg = Interpolator<T>::clamp(seg, 0, getNumSegments() - 1);
	return mSegInterpolators[seg]->getControlPoints();
}

	}
}
