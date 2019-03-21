#include "cHUDHandle.h"

#include "cHUDView.h"

#include <QPainter>


cHUDHandle::~cHUDHandle()
{
}


cHUDHandle::cHUDHandle( cHUDView* iParentView, cHUDObject* iParentObject ) :
    cHUDObject( iParentView, iParentObject )
{
    mColor = Qt::red;
}


void
cHUDHandle::Draw( QPainter* iPainter )
{
    QPen pen( mColor );
    pen.setWidth( 1 );
    iPainter->setPen( pen );

    ResetTransformation();
    CenterScale( mOriginalFrame.center(), 1/GlobalXScale(), 1/GlobalYScale() );

    iPainter->drawRect( GetFinalTransform().mapRect( mOriginalFrame ) );
}


bool
cHUDHandle::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}

