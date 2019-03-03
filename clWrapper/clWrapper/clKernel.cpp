#include "clKernel.h"

void clKernel::RunCallbacks(clEvent KernelFinished)
{
	for (int arg = 0; arg < NumberOfArgs; arg++) {
		if (ArgType[arg] == ArgumentType::Output || ArgType[arg] == ArgumentType::InputOutput) {
			Callbacks[arg]->Update(KernelFinished);
		}
		else if (ArgType[arg] == ArgumentType::OutputNoUpdate || ArgType[arg] == ArgumentType::InputOutputNoUpdate) {
			Callbacks[arg]->UpdateEventOnly(KernelFinished);
		}
	}
}

clEvent clKernel::Do(clWorkGroup Global) {
	std::vector<cl::Event> eventwaitlist;

	// Check callbacks for any input types... need to wait on there write events..
	for (int arg = 0; arg < NumberOfArgs; arg++) {
		// Data is being written to an input type, wait for it to finish
		if (ArgType[arg] == ArgumentType::Input || ArgType[arg] == ArgumentType::InputOutput || ArgType[arg] == ArgumentType::InputOutputNoUpdate) {
			clEvent e = Callbacks[arg]->GetFinishedWriteEvent();
			if (e.GetEvent()())
				eventwaitlist.push_back(e.GetEvent());
		}
		// Current data is presently being retrieved (don't overwrite yet)
		if (ArgType[arg] == ArgumentType::Output || ArgType[arg] == ArgumentType::InputOutput || ArgType[arg] == ArgumentType::OutputNoUpdate || ArgType[arg] == ArgumentType::InputOutputNoUpdate) {
			clEvent e = Callbacks[arg]->GetFinishedReadEvent();
			if (e.GetEvent()())
				eventwaitlist.push_back(e.GetEvent());
		}
	}

	clEvent KernelFinished;
	cl_int status = Context.GetQueue().enqueueNDRangeKernel(Kernel, cl::NullRange, Global.worksize, cl::NullRange, &eventwaitlist, &KernelFinished.GetEvent());
	clError::Throw(status, Name);

	RunCallbacks(KernelFinished);
	return KernelFinished;
}

clEvent clKernel::Do(clWorkGroup Global, clWorkGroup Local) {
	std::vector<cl::Event> eventwaitlist;

	// Check callbacks for any input types... need to wait on there write events..
	for (int arg = 0; arg < NumberOfArgs; arg++) {
		// Data is being written to an input type, wait for it to finish
		if (ArgType[arg] == ArgumentType::Input || ArgType[arg] == ArgumentType::InputOutput || ArgType[arg] == ArgumentType::InputOutputNoUpdate) {
			clEvent e = Callbacks[arg]->GetFinishedWriteEvent();
			if (e.GetEvent()())
				eventwaitlist.push_back(e.GetEvent());
		}
		// Current data is presently being retrieved (don't overwrite yet)
		if (ArgType[arg] == ArgumentType::Output || ArgType[arg] == ArgumentType::InputOutput || ArgType[arg] == ArgumentType::OutputNoUpdate || ArgType[arg] == ArgumentType::InputOutputNoUpdate) {
			clEvent e = Callbacks[arg]->GetFinishedReadEvent();
			if (e.GetEvent()())
				eventwaitlist.push_back(e.GetEvent());
		}
	}

	clEvent KernelFinished;
	cl_int status = Context.GetQueue().enqueueNDRangeKernel(Kernel, cl::NullRange, Global.worksize, Local.worksize, &eventwaitlist, &KernelFinished.GetEvent());
	clError::Throw(status, Name);

	RunCallbacks(KernelFinished);
	return KernelFinished;
}