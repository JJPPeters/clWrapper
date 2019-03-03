#ifndef CL_STATIC_H
#define CL_STATIC_H

#include <list>
#include "clDevice.h"
#include "clContext.h"

namespace Queue
{
	enum QueueType
	{
		InOrder = 0,
		InOrderWithProfiling = 0 | CL_QUEUE_PROFILING_ENABLE,
		OutOfOrder = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
		OutOfOrderWithProfiling = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE
	};
};

class OpenCL
{
public:
	static std::vector<clDevice> GetDeviceList(Device::DeviceType dev_type = Device::DeviceType::All);

	static clContext MakeContext(clDevice &dev, Queue::QueueType Qtype = Queue::QueueType::InOrder);
	static clContext MakeTwoQueueContext(clDevice& dev, Queue::QueueType Qtype = Queue::QueueType::InOrder, Queue::QueueType IOQtype = Queue::QueueType::InOrder);

};

#endif