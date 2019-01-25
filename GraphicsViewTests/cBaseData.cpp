#include "cBaseData.h"


cBaseData::~cBaseData()
{
}


cBaseData::cBaseData()
{
}


void
cBaseData::RegisterEditionCallback( std::function<void( cBaseData*iSender, int iArg )> iCB )
{
    mEditionCBList.push_back( iCB );
}


void
cBaseData::EmitValueChanged( int iArg )
{
    for( auto cb : mEditionCBList )
    {
        cb( this, iArg );
    }
}

