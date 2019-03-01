#pragma once

#include <functional>
#include <vector>

class cThread;
class cThreadHandle;
struct cRange;

class cThreadProcessor
{
public:
    // Construction/Destruction
    ~cThreadProcessor();
    cThreadProcessor();

public:
    static cThreadProcessor* Instance();

public:
    void Initialize();
    void Finalize();

public:
    unsigned int GetAvailableThreadCount() const;

public:
    // Thread execution
    cThreadHandle  AffectFunctionToThreadAndStart( std::function< void( cRange, cRange ) > iFunction, bool iBlockant );
    cThreadHandle  AffectFunctionToThreadAndStart( std::function< void( cRange, cRange ) > iFunction,  cRange iOffset, cRange iRange, bool iBlockant );

private:
    cThreadHandle _SetThreadToWork( int iThreadIndex, std::function< void( cRange, cRange ) > iFunction, cRange iOffset, cRange iRange );

protected:
    std::vector< cThread* >  mThreads;
    unsigned int            mTotalCoreCount;

};

