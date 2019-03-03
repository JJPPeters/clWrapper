#ifndef CL_FOURIER_H
#define CL_FOURIER_H

#include "clContext.h"
#include "CL/cl.hpp"
#include "boost/shared_ptr.hpp"
#include "clFFT.h"
#include "clEvent.h"
#include "clMemory.h"

class AutoTeardownFFT;
template <class T, template <class> class AutoPolicy> class clMemory;
template <class T> class Manual;

// Can't use scoped enums for legacy code
namespace Direction
{
	enum TransformDirection
	{
		Forwards,
		Inverse
	};
};

class clFourier
{
	clContext Context;
	clfftSetupData fftSetupData;
	clfftPlanHandle fftplan;
	
	//intermediate buffer	
	clMemory<char, Manual> clMedBuffer;
	unsigned int width, height;
	size_t buffersize;

	void Setup(unsigned int _width, unsigned int _height);

public:

	clFourier() : fftplan(0), width(0), height(0) {}

	clFourier(clContext Context, unsigned int _width, unsigned int _height);

	clFourier(const clFourier &RHS) : Context(RHS.Context), fftplan(0), width(RHS.width), height(RHS.height), buffersize(0) {
		if (Context.GetContext()())
			Setup(width, height);
	};

	~clFourier();

	clFourier& operator=(const clFourier &RHS) {
		if (this != &RHS) {
			if (fftplan) {
				clfftStatus fftStatus = clfftDestroyPlan(&fftplan);
				clFftError::Throw(fftStatus, "clFourier");
			}
			fftplan = 0;
			Context = RHS.Context;
			width = RHS.width;
			height = RHS.height;
			if (Context.GetContext()())
				Setup(width, height);
		}
		return *this;
	};

	template <class T, template <class> class AutoPolicy, template <class> class AutoPolicy2>
	clEvent Do(clMemory<T, AutoPolicy2>& input, clMemory<T, AutoPolicy>& output, Direction::TransformDirection Direction)
	{
		clfftDirection Dir = (Direction == Direction::Forwards) ? CLFFT_FORWARD : CLFFT_BACKWARD;

		std::vector<cl_event> eventwaitlist;
		clEvent e = input.GetFinishedWriteEvent();
		clEvent e2 = input.GetFinishedReadEvent();
		if (e.GetEvent()())
			eventwaitlist.push_back(e.GetEvent()());
		if (e2.GetEvent()())
			eventwaitlist.push_back(e2.GetEvent()());

		clEvent finished;

		if (buffersize)
			fftStatus = clfftEnqueueTransform(fftplan, Dir, 1, &Context.GetQueue()(), (cl_uint)eventwaitlist.size(),
				!eventwaitlist.empty() ? &eventwaitlist[0] : NULL, &finished.event(),
				&input.GetBuffer()(), &output.GetBuffer()(), clMedBuffer.GetBuffer()());
		else
			fftStatus = clfftEnqueueTransform(fftplan, Dir, 1, &Context.GetQueue()(), (cl_uint)eventwaitlist.size(),
				!eventwaitlist.empty() ? &eventwaitlist[0] : NULL, &finished.GetEvent()(),
				&input.GetBuffer()(), &output.GetBuffer()(), NULL);

		if (output.getAuto())
			output.Update(finished);

		return finished;
	};

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }
};

// Singleton to auto call clfftteardown on program termination
class AutoTeardownFFT
{
private:
	AutoTeardownFFT &operator=(AutoTeardownFFT const &rhs);
	AutoTeardownFFT(AutoTeardownFFT const& copy);

public:
	AutoTeardownFFT() {}

	~AutoTeardownFFT() {
		clfftStatus status = clfftTeardown();
		clFftError::Throw(status, "FourierTearDown");
	}
	inline static AutoTeardownFFT& GetInstance() { static AutoTeardownFFT instance; return instance; }
};

#endif

