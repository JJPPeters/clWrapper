#include "clDevice.h"

Device::DeviceType clDevice::getDeviceType() {
	cl_int status;
	Device::DeviceType deviceType = static_cast<Device::DeviceType>(device.getInfo<CL_DEVICE_TYPE>(&status));
	clError::Throw(status, "clDevice");
	return deviceType;
};