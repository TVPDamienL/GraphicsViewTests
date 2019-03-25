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

    QRectF frameTransf = GetFinalTransform().mapRect( mOriginalFrame );

    float offsetX = (frameTransf.width() - HANDLESIZE) / 2;
    float offsetY = (frameTransf.height() - HANDLESIZE) / 2;

    frameTransf.setWidth( HANDLESIZE );
    frameTransf.setHeight( HANDLESIZE );
    frameTransf.translate( QPointF( offsetX, offsetY ) );

    iPainter->drawRect( frameTransf );
}


bool
cHUDHandle::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}

