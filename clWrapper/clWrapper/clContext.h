#ifndef CL_CONTEXT_H
#define CL_CONTEXT_H

#include <list>

#include "CL/cl.hpp"
#include "boost/shared_ptr.hpp"

#include "clerror.h"
#include "clDevice.h"

template <class T, template <class> class AutoPolicy> class clMemory;

enum MemoryFlags {
	ReadWrite = CL_MEM_READ_WRITE,
	ReadOnly = CL_MEM_READ_ONLY,
	WriteOnly = CL_MEM_WRITE_ONLY
};

class MemoryRecord {
public:
	MemoryRecord(size_t _size): size(_size){};
	size_t size;
};

class clContext {
private:
	cl::Context Context;
	cl::CommandQueue Queue;
	cl::CommandQueue IOQueue;
	clDevice ContextDevice;

	std::vector<boost::shared_ptr<MemoryRecord>> MemList;

public:
	clContext() {}

	clContext(cl::Context _context, cl::CommandQueue _queue, clDevice _device)
		: Context(_context), Queue(_queue), IOQueue(_queue), ContextDevice(_device) {}
	clContext(cl::Context _context, cl::CommandQueue _queue, cl::CommandQueue _ioqueue, clDevice _device)
		: Context(_context), Queue(_queue), IOQueue(_ioqueue), ContextDevice(_device) {}

	~clContext() {};

	void WaitForQueueFinish() {
		int status = Queue.finish();
		clError::Throw(status);
	}
	void WaitForIOQueueFinish() {
		int status = IOQueue.finish();
		clError::Throw(status);
	}
	void QueueFlush() {
		int status = Queue.flush();
		clError::Throw(status);
	}
	void IOQueueFlush() {
		int status = IOQueue.flush();
		clError::Throw(status);
	}

	clDevice& GetContextDevice() { return ContextDevice; }
	cl::Context& GetContext() { return Context; }
	cl::CommandQueue& GetQueue() { return Queue; }
	cl::CommandQueue& GetIOQueue() { return IOQueue; }

	size_t GetOccupiedMemorySize() {
		size_t total = 0;
		for (int i = 0; i < MemList.size(); ++i)
			total += MemList[i]->size;
		return total;
	}

	void AddMemRecord(const boost::shared_ptr<MemoryRecord> &rec) {
		std::vector<boost::shared_ptr<MemoryRecord>>::iterator it = std::find(MemList.begin(), MemList.end(), rec);
		if (it == MemList.end())
			MemList.push_back(rec);
	}

	void RemoveMemRecord(const boost::shared_ptr<MemoryRecord> &rec) {
		std::vector<boost::shared_ptr<MemoryRecord>>::iterator it = std::find(MemList.begin(), MemList.end(), rec);
		if (it != MemList.end())
			MemList.erase(it);
	}
};
#endif