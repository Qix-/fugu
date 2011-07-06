#include "fg/turtle.h"

#include <iostream>

namespace fg {
	namespace gc {
	Turtle::Turtle()
	{
		mFrame.SetIdentity();
	}

	void Turtle::move(double distance)
	{
		Mat4 trans;
		trans.SetTranslate(0.,0.,distance);
		mFrame = trans*mFrame;
	}

	void Turtle::beginCarrier()
	{
		mPrevFrames.clear();
		mPrevFrames.push_back(mFrame);
	}

	void Turtle::addPoint()
	{
		mPrevFrames.push_back(mFrame);
	}

	CarrierCurveLinear Turtle::endCarrier()
	{
		mPrevFrames.push_back(mFrame);

		Mat4 cp[mPrevFrames.size()];
		for (int i = 0; i < mPrevFrames.size(); ++i)
		{
			cp[i] = mPrevFrames[i];
		}

		return CarrierCurveLinear(mPrevFrames.size(), cp);
	}
	}
}
