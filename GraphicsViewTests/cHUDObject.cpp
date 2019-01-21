#include "cHUDObject.h"


cHUDObject::~cHUDObject()
{
}


cHUDObject::cHUDObject()
{
}


void
cHUDObject::SetFrame( const QRect & iFrame )
{
    mFrame = iFrame;
}


void
cHUDObject::MoveBy( const QPoint & iOffset )
{
    mFrame.translate( iOffset );
}

