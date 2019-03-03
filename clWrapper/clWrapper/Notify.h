#ifndef NOTIFY_H
#define NOTIFY_H

#include "clEvent.h"

class Notify
{
public:
	virtual void Update(clEvent KernelFinished)=0;
	virtual void UpdateEventOnly(clEvent KernelFinished) = 0;
	virtual clEvent GetFinishedWriteEvent()=0;
	virtual clEvent GetFinishedReadEvent()=0;
};

#endif