#pragma once

#include <CL/cl.hpp>

#include <unordered_map>
#include <string>


class GPUController
{
public:
    ~GPUController();
    GPUController();

public:
    cl::Buffer* NewBuffer( size_t iSizeInBytes, cl_mem_flags iType );
    void        DeleteBuffer( cl::Buffer* iBuffer );

    void        RegisterShader( const std::string& iPathToFile );
    void        BuildKernel( const std::string& iKernelName );
    cl::Kernel* GetKernel( const std::string& iKernelName );

    cl::Program*GetProgram();
    cl_int      BuildProgram();

protected:
    cl::Platform                                    mPlatform;
    cl::Device                                      mDevice;
    cl::Context*                                    mContext;
    cl::Program::Sources*                           mShadersSource;
    cl::Program*                                    mProgram;
    std::unordered_map< std::string, cl::Kernel* >  mKernels;


private:
    std::vector< cl::Buffer* >                      mAllocatedBuffers;
    cl_ulong                                        _mTotalAvailableMemory;
    cl_ulong                                        _mMemoryLeftAvailabe;

    std::vector< std::string >                      _mTMPSourceBuffer;
};
