#ifndef CL_WORKGROUP_H
#define CL_WORKGROUP_H

#include "CL/cl.hpp"

// Specifiy number of threads to launch
class clWorkGroup
{
public:
	clWorkGroup(unsigned int x, unsigned int y, unsigned int z) {
		worksize = cl::NDRange(x, y, z);
	};

	clWorkGroup(unsigned int x, unsigned int y) {
		worksize = cl::NDRange(x, y);
	};

	clWorkGroup(unsigned int x) {
		worksize = cl::NDRange(x);
	};

	cl::NDRange worksize;
};

#endif