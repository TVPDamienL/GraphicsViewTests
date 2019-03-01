#include "ThreadMachine.h"


#include "Thread.h"


#include <thread>


cThreadProcessor::~cThreadProcessor()
{
}


cThreadProcessor::cThreadProcessor()
{
}


cThreadProcessor*
cThreadProcessor::Instance()
{
    static cThreadProcessor* gThreadProcessor = 0;
    if( !gThreadProcessor )
    {
        gThreadProcessor = new cThreadProcessor();
        gThreadProcessor->Initialize();
    }

    return  gThreadProcessor;
}


void
cThreadProcessor::Initialize()
{
    mTotalCoreCount = std::thread::hardware_concurrency();
    if( mTotalCoreCount > 1 )
    {
        for( unsigned int i = 0; i < mTotalCoreCount; ++i )
        {
            mThreads.push_back( new cThread() );
            mThreads.back()->CreateAndLaunchThread();
        }
    }
}


void
cThreadProcessor::Finalize()
{
    for( int i = 0; i < mThreads.size(); ++i )
    {
        cThread* thread = mThreads[ i ];

        thread->SetThreadFunction( 0, cRange( 0, 0 ), cRange( 0, 0 ) );
        thread->Stop();

        std::unique_lock< std::mutex > lock( thread->IdleMutex() );
        thread->UnlockThread();
        lock.unlock();
        thread->CV().notify_one();
    }

    for( int i = 0; i < mThreads.size(); ++i )
    {
        mThreads[ i ]->Join();
    }

    for( int i = 0; i < mThreads.size(); ++i )
    {
        delete mThreads[ i ];
    }
}


unsigned int
cThreadProcessor::GetAvailableThreadCount() const
{
    unsigned int count = 0;
    for( int i = 0; i < mThreads.size(); ++i )
    {
        if( mThreads[ i ]->Locked() )
            ++count;
    }

    return  count;
}


cThreadHandle
cThreadProcessor::AffectFunctionToThreadAndStart( std::function< void( cRange, cRange ) > iFunction, bool iBlockant )
{
    return  AffectFunctionToThreadAndStart( iFunction, cRange( 0, 0 ), cRange( 0, 0 ), iBlockant );
}


cThreadHandle
cThreadProcessor::AffectFunctionToThreadAndStart( std::function<void( cRange, cRange )> iFunction, cRange iOffset, cRange iRange, bool iBlockant )
{
    if( mTotalCoreCount == 1 )
    {
        iFunction( iOffset, iRange );
        return  cThreadHandle( 0 ); // No thread available
    }

    for( int i = 0; i < mThreads.size(); ++i )
    {
        if( mThreads[ i ]->Locked() )
            return  _SetThreadToWork( i, iFunction, iOffset, iRange );
    }

    // From here, all thread are busy, we can either be non blocant, and return
    // Or, we can block and wait for a thread
    while( iBlockant )
    {
        for( int i = 0; i < mThreads.size(); ++i )
        {
            if( mThreads[ i ]->Locked() )
                return  _SetThreadToWork( i, iFunction, iOffset, iRange );
        }
    }

    return  cThreadHandle( 0 ); // No thread available
}


cThreadHandle
cThreadProcessor::_SetThreadToWork( int iThreadIndex, std::function<void( cRange, cRange )> iFunction, cRange iOffset, cRange iRange )
{
    cThread* thread = mThreads[ iThreadIndex ];
    std::unique_lock< std::mutex > lock( thread->IdleMutex() );

    thread->SetThreadFunction( iFunction, iOffset, iRange );

    thread->UnlockThread();
    lock.unlock();
    thread->CV().notify_one();

    return  cThreadHandle( thread );
}


