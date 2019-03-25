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
    QRect circleRect( center.x() - mSize/2, center.y() - mSize/2, mSize, mSize );
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


void
cHUDHandlePath::setSize( float size )
{
    mSize = size;
    QPoint handleCenter( mOriginalFrame.center().x(), mOriginalFrame.center().y() );
    QRect handleRect( handleCenter.x() - HANDLESIZE/2 + mSize/2, handleCenter.y() - HANDLESIZE/2, HANDLESIZE, HANDLESIZE );

    mPressureHandle->SetFrame( handleRect );
}


void
cHUDHandlePath::SetFrame( const QRectF & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    QPoint handleCenter( mOriginalFrame.center().x(), mOriginalFrame.center().y() );
    QRect handleRect( handleCenter.x() - HANDLESIZE/2 + mSize/2, handleCenter.y() - HANDLESIZE/2, HANDLESIZE, HANDLESIZE );

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


cHUDObject*
cHUDHandlePath::GetVisibleHUDObjectAtPos( const QPointF & iPoint )
{
    return nullptr;
}

