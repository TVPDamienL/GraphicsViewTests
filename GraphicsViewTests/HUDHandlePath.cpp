#include "HUDHandlePath.h"

#include "cHUDView.h"
#include "cHUDHandle.h"

#include <QPainter>


cHUDHandlePath::~cHUDHandlePath()
{
}


cHUDHandlePath::cHUDHandlePath( cHUDView* iParentView, cHUDObject* iParentObject ) :
    cHUDObject( iParentView, iParentObject )
{
    mColor = Qt::red;
    mPressureHandle = new cHUDHandle( iParentView, this );
    mPressureHandle->mColor = Qt::blue;
    mChildrenHUDs.push_back( mPressureHandle );
}


void
cHUDHandlePath::Draw( QPainter* iPainter )
{
    QPen pen( mColor );
    pen.setWidth( 1 );
    iPainter->setPen( pen );

    QTransform parentsTransforms = mParentView->GetTransform();

    QPointF center = mOriginalFrame.center();
    const int size = mOriginalFrame.width();
    QRect circleRect( center.x() - size/2, center.y() - size /2, size , size  );
    iPainter->drawEllipse( parentsTransforms.mapRect( circleRect ) );
    //iPainter->drawRect( parentsTransforms.mapRect( mPressureHandle->GetFrame() ) );



    QRectF frameTransf = GetFinalTransform().mapRect( mOriginalFrame );

    float offsetX = (frameTransf.width() - HANDLESIZE) / 2;
    float offsetY = (frameTransf.height() - HANDLESIZE) / 2;

    frameTransf.setWidth( HANDLESIZE );
    frameTransf.setHeight( HANDLESIZE );
    frameTransf.translate( QPointF( offsetX, offsetY ) );

    iPainter->drawRect( frameTransf );

    mPressureHandle->Draw( iPainter );
}


bool
cHUDHandlePath::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}


bool
cHUDHandlePath::ContainsPoint( const QPointF & iPoint ) const
{
    for( auto handle : mChildrenHUDs )
    {
        if( handle->ContainsPoint( iPoint ) )
            return  true;
    }

    return  cHUDObject::ContainsPoint( iPoint );
}


void
cHUDHandlePath::SetFrame( const QRectF & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    const float size = iFrame.width();
    QPointF handleCenter( iFrame.center().x(), iFrame.center().y() );
    QRect handleRect( handleCenter.x() - HANDLESIZE/2 + size/2, handleCenter.y() - HANDLESIZE/2, HANDLESIZE, HANDLESIZE );

    mPressureHandle->SetFrame( handleRect );
}


QRectF
cHUDHandlePath::GetFrame() const
{
    return  cHUDObject::GetFrame();
}


const cHUDHandle *
cHUDHandlePath::GetHandlePtr() const
{
    return  mPressureHandle;
}
