#pragma once


#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>


struct cRange // Like NDRange in opencl
{
    cRange( int x, int y ) : mX(x), mY(y) {}
    int mX;
    int mY;
};


class cThread
{
public:
    enum eThreadState
    {
        kIdle,
        kBusy
    };

public:
    // Construction/Destruction
    ~cThread();
    cThread();

public:
    // Accessors
    void  SetThreadFunction( std::function< void( cRange, cRange ) > iFunction, cRange iOffset, cRange iRange );

    std::condition_variable&  CV();
    std::mutex&     IdleMutex();
    void            UnlockThread();
    unsigned int    ID()        const;
    eThreadState    State()     const;
    bool            Locked()    const;


public:
    // Thread operations
    void  CreateAndLaunchThread();
    void  Stop();
    void  Join();
    void  WaitEndOfTask();

private:
    // Private functions
    void RunningFunction();

protected:
    std::thread             mThread;
    unsigned int            mID;

    std::mutex              mIdleMutex;
    std::condition_variable mCV;

    std::function< void( cRange, cRange ) > mThreadFunction;
    cRange                                  mThreadFunctionOffset;
    cRange                                  mThreadFunctionRange;

    eThreadState            mState;
    bool                    mLocked;
    bool                    mRunning;

};


//=======================================================================================
//=======================================================================================
//=======================================================================================


class cThreadHandle
{
public:
    ~cThreadHandle();
    cThreadHandle( cThread* iThread );

public:
    bool IsValid() const;
    cThread* GetThread();

private:
    cThread * mThread;
    unsigned int mThreadID;

};

