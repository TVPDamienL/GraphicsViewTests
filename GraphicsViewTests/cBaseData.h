#pragma once

#include <functional>

class cBaseData
{
public:
    ~cBaseData();
    cBaseData();

public:
    unsigned int    RegisterEditionCallback( std::function< void( cBaseData* iSender, int iArg ) > iCB );
    void            UnregisterEditionCallback( unsigned int iCBID );

public:
    void EmitValueChanged( int iArg );

private:
    std::unordered_map< unsigned int, std::function< void( cBaseData* iSender, int iArg ) > > mEditionCBList;
};

