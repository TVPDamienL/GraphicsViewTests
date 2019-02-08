#include "cBaseData.h"


// An id to unregister later, as we only store lambdas, and can't unregister em any other way
static unsigned int sIDCounter = 0;

cBaseData::~cBaseData()
{
}


cBaseData::cBaseData()
{
}


unsigned int
cBaseData::RegisterEditionCallback( std::function<void( cBaseData*iSender, int iArg )> iCB )
{
    ++sIDCounter;

    mEditionCBList[ sIDCounter ] = iCB;
    return  sIDCounter;
}


void
cBaseData::UnregisterEditionCallback( unsigned int iCBID )
{
    mEditionCBList.erase( iCBID );
}


void
cBaseData::EmitValueChanged( int iArg )
{
    for( auto pair : mEditionCBList )
    {
        pair.second( this, iArg );
    }
}

