#ifndef CL_EVENT_H
#define CL_EVENT_H

#include "CL/cl.hpp"

#include "clerror.h"

// Used to synchronise OpenCL functions that depend on other results.
class clEvent
{
private:
	cl::Event event;

public:
	clEvent() {};

	void Wait()
	{
		cl_int status;
		status = event.wait();
		clError::Throw(status);
	}

	cl::Event GetEvent() {
		return event;
	}
};

#endif