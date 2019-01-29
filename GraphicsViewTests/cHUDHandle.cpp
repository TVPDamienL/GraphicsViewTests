#include "cHUDHandle.h"

#include "cHUDView.h"

#include <QPainter>


cHUDHandle::~cHUDHandle()
{
}


cHUDHandle::cHUDHandle( cHUDView* iParentView, cHUDObject* iParentObject ) :
    cHUDObject( iParentView, iParentObject )
{
}


void
cHUDHandle::Draw( QPainter* iPainter )
{
    QPen pen( Qt::red );
    pen.setWidth( 1 );
    iPainter->setPen( pen );

    ResetTransformation();

    iPainter->drawRect( ToHUDCoords( mOriginalFrame ) );

    CenterScale( mOriginalFrame.center(), 1/mParentView->Scale() );

    iPainter->setPen( Qt::blue );
    iPainter->drawRect( ToHUDCoords( mOriginalFrame ) );
}


bool
cHUDHandle::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}

