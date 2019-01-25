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


    QRect drawingFrame = mParentView->MapToView( mOriginalFrame );
    const float inv = 1/mParentView->Scale();

    // Center scale that cancels the parentView's scale, so handles are always the same size
    mObjectSelfTransformation.reset();
    mObjectSelfTransformation.translate( drawingFrame.center().x(), drawingFrame.center().y() );
    mObjectSelfTransformation.scale( inv, inv );
    mObjectSelfTransformation.translate( -drawingFrame.center().x(), -drawingFrame.center().y() );

    iPainter->drawRect(  mObjectSelfTransformation.mapRect( drawingFrame ) );
}


bool
cHUDHandle::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}

