clWrapper
=========

OpenCL Wrapper for C++ to provide easier access to context and queue management, 
memory allocation/deallocation and kernel invoking.

This wrapper has largely been written for use in my simulation programs, as well as in Gatan Microscopy Suite (hence the support for C++03)

### Features

- Memory buffers will automatically deallocate themselves when they go out of scope.
- Buffer types are templated to show what type they are expected to contain.
- Buffers can be set to automatically retrieve contents to host storage when they get modified by a kernel or to operate manually.
- Events are also automatically created to allow for all memory syncing to be performed in a seperate queue automatically which allows for automatic asynchronous memory transfers wherever possible.
- Includes second project which uses Boost.Test to verify wrapper is working properly.

