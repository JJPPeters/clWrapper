#ifndef CLWRAPPER_MAIN_CLERROR_H
#define CLWRAPPER_MAIN_CLERROR_H

#include <string>

#include "CL/cl.hpp"

#include "clFFT.h"

namespace clError
{
	std::string GetError(cl_int error_code);

	void Throw(cl_int error_code, std::string info = "-");
}

namespace clFftError
{
	std::string GetError(clfftStatus error_code);

	void Throw(clfftStatus error_code, std::string info = "-");
}

#endif //CLWRAPPER_MAIN_CLERROR_H
