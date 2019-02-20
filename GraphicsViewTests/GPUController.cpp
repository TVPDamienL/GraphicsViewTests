#include "GPUController.h"

#include <fstream>
#include <sstream>

#include <QDebug>

GPUController::~GPUController()
{
    delete  mContext;
    for( auto pair : mKernels )
        delete  pair.second;

    for( auto buffer : mAllocatedBuffers )
        delete  buffer;

    delete  mProgram;
    delete  mShadersSource;
}


GPUController::GPUController()
{
    mProgram = 0;
    mShadersSource = new cl::Program::Sources();

    // Get all platforms (drivers)
    std::vector< cl::Platform > allPlatforms;
    cl::Platform::get( &allPlatforms );
    if( allPlatforms.size() == 0 )
        return;

    mPlatform = allPlatforms[0];

    //get default device of the default platform
    std::vector< cl::Device > allDevices;
    mPlatform.getDevices( CL_DEVICE_TYPE_ALL, &allDevices );
    if( allDevices.size() == 0 )
        return;

    mDevice = allDevices[0];
    mContext = new cl::Context( { mDevice } );

    mDevice.getInfo( CL_DEVICE_GLOBAL_MEM_SIZE, &_mTotalAvailableMemory );
    _mMemoryLeftAvailabe = _mTotalAvailableMemory;
}


cl::Buffer*
GPUController::NewBuffer( size_t iSizeInBytes, cl_mem_flags iType )
{
    if( _mMemoryLeftAvailabe - iSizeInBytes < 0 )
        return  0;

    cl::Buffer* buffer = new cl::Buffer( *mContext, iType, iSizeInBytes );

    // To ensure exact values, we read the actual value it got
    size_t mem;
    buffer->getInfo( CL_MEM_SIZE, &mem );
    _mMemoryLeftAvailabe -= mem;

    mAllocatedBuffers.push_back( buffer );

    return  buffer;
}


void
GPUController::DeleteBuffer( cl::Buffer * iBuffer )
{
    for( int i = 0; i < mAllocatedBuffers.size(); ++i )
    {
        if( mAllocatedBuffers[ i ] == iBuffer )
        {
            delete  iBuffer;
            mAllocatedBuffers.erase( mAllocatedBuffers.begin() + i );

            size_t mem;
            iBuffer->getInfo( CL_MEM_SIZE, &mem );
            _mMemoryLeftAvailabe += mem;
        }
    }
}


void
GPUController::RegisterShader( const std::string & iPathToFile )
{
    std::ifstream myfile;
    myfile.open( iPathToFile );
    if( !myfile.is_open() )
    {
        qDebug() << "Can't load " << iPathToFile.c_str();
        return;
    }

    std::stringstream buffer;
    buffer << myfile.rdbuf();
    _mTMPSourceBuffer.push_back( buffer.str() ); // Because shader source won't copy the source's string ... so we need a persistent string

    mShadersSource->push_back( {_mTMPSourceBuffer.back().c_str(), _mTMPSourceBuffer.back().length()} );
}


void
GPUController::BuildKernel( const std::string & iKernelName )
{
    if( mKernels[ iKernelName ] )
        delete  mKernels[ iKernelName ];

    mKernels[ iKernelName ] = new cl::Kernel( *mProgram, iKernelName.c_str() );
}


cl::Kernel*
GPUController::GetKernel( const std::string & iKernelName )
{
    return  mKernels[ iKernelName ];
}


cl::Program*
GPUController::GetProgram()
{
    return  mProgram;
}


cl_int
GPUController::BuildProgram()
{
    delete  mProgram;
    mProgram = new cl::Program( *mContext, *mShadersSource );
    cl_int error = mProgram->build( {mDevice} );
    if( mProgram->build( { mDevice } ) != CL_SUCCESS )
    {
        delete  mProgram;
        mProgram = 0;
        return  error;
    }

    _mTMPSourceBuffer.clear();
    return  CL_SUCCESS;
}
