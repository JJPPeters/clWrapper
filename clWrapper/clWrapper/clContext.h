#ifndef CL_CONTEXT_H
#define CL_CONTEXT_H

#include "clMemory.h"
#include <list>
#include "clDevice.h"
#include "CL/Opencl.h"
#include "boost/shared_ptr.hpp"

template <class T, template <class> class AutoPolicy> class clMemory;

enum MemoryFlags
{
	ReadWrite = CL_MEM_READ_WRITE,
	ReadOnly = CL_MEM_READ_ONLY,
	WriteOnly = CL_MEM_WRITE_ONLY
};

class MemoryRecord
{
public:
	MemoryRecord(size_t _size): size(_size){};
	size_t size;
};

class clContext
{

private:
	cl_int Status;
	cl_context Context;
	cl_command_queue Queue;
	clDevice ContextDevice;
	std::list<MemoryRecord*> MemList;

public:
	clContext(clDevice _ContextDevice, cl_context _Context, cl_command_queue _Queue, cl_int _Status)
		: ContextDevice(_ContextDevice), Context(_Context), Queue(_Queue), Status(_Status){};



	void WaitForQueueFinish(){clFinish(Queue);};
	void QueueFlush(){clFlush(Queue);};

	clDevice GetContextDevice(){return ContextDevice;};
	cl_context& GetContext(){return Context;};
	cl_int GetStatus(){return Status;};
	cl_command_queue& GetQueue(){ return Queue; };
	virtual cl_command_queue& GetIOQueue(){return Queue;};

	size_t GetOccupiedMemorySize()
	{
		std::list<MemoryRecord*>::iterator it; size_t total = 0;
		for(it = MemList.begin(); it != MemList.end(); it++)
		{
			total += (*it)->size;
		}
		return total;
	}

	void RemoveMemRecord(MemoryRecord* rec)
	{
		MemList.remove(rec);
	}

	template<class T,template <class> class AutoPolicy> boost::shared_ptr<clMemory<T,AutoPolicy>> CreateBuffer(size_t size)
	{
		MemoryRecord* rec = new MemoryRecord(size*sizeof(T));
		boost::shared_ptr<clMemory<T,AutoPolicy>> Mem( new clMemory<T,AutoPolicy>(this,size,clCreateBuffer(Context, MemoryFlags::ReadWrite, size*sizeof(T), 0, &Status),rec));
		MemList.push_back(rec);
		return Mem;
	};

	template<class T,template <class> class AutoPolicy > boost::shared_ptr<clMemory<T,AutoPolicy>> CreateBuffer(size_t size, enum MemoryFlags flags)
	{
		MemoryRecord* rec = new MemoryRecord(size*sizeof(T));
		boost::shared_ptr<clMemory<T,AutoPolicy>> Mem( new clMemory<T,AutoPolicy>(this,size,clCreateBuffer(Context, flags, size*sizeof(T), 0, &Status),rec));
		MemList.push_back(rec);
		return Mem;
	};

};


#endif