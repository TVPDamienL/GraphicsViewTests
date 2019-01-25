#pragma once

#include <functional>

class cBaseData
{
public:
    ~cBaseData();
    cBaseData();

public:
    void RegisterEditionCallback( std::function< void( cBaseData* iSender, int iArg ) > iCB );

public:
    void EmitValueChanged( int iArg );

private:
    std::vector< std::function< void( cBaseData* iSender, int iArg ) > > mEditionCBList;
};

