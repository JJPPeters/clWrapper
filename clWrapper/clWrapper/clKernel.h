#ifndef CL_KERNEL_H
#define CL_KERNEL_H

#include <algorithm>
#include <utility>

#include "CL/cl.hpp"

#include "Utilities.h"
#include "clMemory.h"
#include "clEvent.h"
#include "clWorkgroup.h"

// Optionally passed to argument setting.
// Output types will be updated automatically when data is modified

namespace ArgumentType
{
	// Unspecified first so containers will default to this value.
	enum ArgTypes
	{
		Unspecified,
		Input,
		Output,
		InputOutput,
		OutputNoUpdate,
		InputOutputNoUpdate
	};
};

class clKernel
{
private:
	clContext Context;
	cl::Program Program;
	cl::Kernel Kernel;
	std::string Name;

	unsigned int NumberOfArgs;
	std::vector<ArgumentType::ArgTypes> ArgType;
	std::vector<Notify*> Callbacks;

	void RunCallbacks(clEvent KernelFinished);

public:
	clKernel() {}

	clKernel(clContext _context, const std::string &codestring, std::string _name, unsigned int _numArgs)
		: Context(_context), Name(_name), NumberOfArgs(_numArgs)
	{
		ArgType.resize(NumberOfArgs);
		Callbacks.resize(NumberOfArgs);

		std::string options = "-cl-finite-math-only -cl-unsafe-math-optimizations -cl-no-signed-zeros";// -Werror";

		cl_int status;
		Program = cl::Program(Context.GetContext(), codestring, false, &status);
		status = Program.build(options.c_str()); // could just put true above - need to remember to pass it the string


		std::string buildlog_str = Program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(Context.GetContextDevice().getDevice(), &status);
		clError::Throw(status, Name + "\nBuild log:\n" + buildlog_str);

		Kernel = cl::Kernel(Program, Name.c_str(), &status);
		clError::Throw(status, Name + "\nBuild log:\n" + buildlog_str);
	}

	template <class T, template <class> class AutoPolicy>
	void SetArg(cl_uint index, clMemory<T, AutoPolicy>& arg, ArgumentType::ArgTypes ArgumentType = ArgumentType::Unspecified) {
		ArgType[index] = ArgumentType;
		Callbacks[index] = &arg;

		cl_int status = Kernel.setArg(index, arg.GetBuffer());
		clError::Throw(status, Name + " arg " + toString(index));
	}

	// Overload for OpenCL Memory Buffers
	template <class T>
	void SetArg(cl_uint index, const T arg, ArgumentType::ArgTypes ArgumentType = ArgumentType::Unspecified) {
		ArgType[index] = ArgumentType;

		cl_int status = Kernel.setArg(index, arg);
		clError::Throw(status, Name + " arg " + toString(index));
	}

	template <class T>
	void SetLocalMemoryArg(cl_uint index, unsigned int size) {
		cl_int status = Kernel.setArg(index, size * sizeof(T), NULL);
		clError::Throw(status, Name + " arg " + toString(index));
	}

	clEvent Do(clWorkGroup Global);
	clEvent Do(clWorkGroup Global, clWorkGroup Local);
};

#endif