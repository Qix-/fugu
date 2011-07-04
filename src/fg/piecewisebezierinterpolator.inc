namespace fg {
	namespace spline {

template<class T>
PiecewiseBezierInterpolator<T>::PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints, const T *gradients )
:Interpolator<T>()
,mSegInterpolators(NULL)
,mGradients(NULL)
{
	setControlPoints(numControlPoints, controlPoints, gradients);
}

template<class T>
PiecewiseBezierInterpolator<T>::PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints)
:Interpolator<T>()
,mSegInterpolators(NULL)
,mGradients(NULL)
{
	setControlPoints(numControlPoints, controlPoints);
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoints(int numControlPoints, const T *controlPoints)
{
	T *gradients = new T[numControlPoints];

    gradients[0] = (controlPoints[1] - controlPoints[0]) * .5f;
    gradients[numControlPoints - 1] = (controlPoints[numControlPoints - 1] - controlPoints[numControlPoints - 2]) * .5f;
	for(int i = 1; i < numControlPoints - 1; ++i)
		{
			gradients[i] = (controlPoints[i + 1] - controlPoints[i-1]) * 0.5f;
		}
	setControlPoints(numControlPoints, controlPoints, gradients);
}

template<class T>
void PiecewiseBezierInterpolator<T>::setControlPoints(int numControlPoints, const T *controlPoints, const T *gradients)
{
	deleteData();
    Interpolator<T>::setControlPoints(numControlPoints, controlPoints);

    mGradients = new T[numControlPoints];
    for (int i = 0; i < numControlPoints; ++i)
    {
		mGradients[i] = gradients[i];
	}

	mSegInterpolators = new BezierInterpolator<T>*[getNumSegments()];
	T cp[4];
    for (int i = 0; i < getNumSegments(); ++i)
    {
		cp[0] = controlPoints[i];
		cp[1] = controlPoints[i] + gradients[i];
		cp[2] = controlPoints[i + 1] - gradients[i];
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
	max = (double) (getNumSegments() - 1);
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
	Interpolator<T>::getNumControlPoints() - 1;
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
    return Interpolator<T>::getControlPoint( 0 );
}

template<class T>
T PiecewiseBezierInterpolator<T>::getSecondDerivative (double t) const
{
    return Interpolator<T>::getControlPoint( 0 );
}

template < class T >
int PiecewiseBezierInterpolator < T >::getSegment(double t) const
{
    int ti = (int) t;
	ti = Interpolator<T>::clamp(ti, 0, getNumSegments() - 1);
    return ti;
}

	}
}
