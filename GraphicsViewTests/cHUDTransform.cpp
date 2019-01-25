#include "cHUDTransform.h"

#include "cHUDView.h"

cHUDTransform::~cHUDTransform()
{
}


cHUDTransform::cHUDTransform( cHUDView* iParentView, cHUDObject* iParentObject ) :
    cHUDObject( iParentView, iParentObject )
{
    auto topLeft = new cHUDHandle( iParentView, this );
    mChildrenHUDs.push_back( topLeft );

    auto topRight = new cHUDHandle( iParentView, this );
    mChildrenHUDs.push_back( topRight );

    auto botRight = new cHUDHandle( iParentView, this );
    mChildrenHUDs.push_back( botRight );

    auto botLeft = new cHUDHandle( iParentView, this );
    mChildrenHUDs.push_back( botLeft );

    _LayoutChildren();
}


void
cHUDTransform::Draw( QPainter * iPainter )
{
    for( auto handle : mChildrenHUDs )
    {
        handle->Draw( iPainter );
    }

    QPen pen( Qt::red );
    pen.setWidth( 1 );
    iPainter->setPen( pen );
    iPainter->drawRect( mParentView->MapToView( mOriginalFrame ) );
}


void
cHUDTransform::SetFrame( const QRect & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    _LayoutChildren();

}


bool
cHUDTransform::ContainsPoint( const QPoint & iPoint ) const
{
    for( auto handle : mChildrenHUDs )
    {
        if( handle->ContainsPoint( iPoint ) )
            return  true;
    }

    return  cHUDObject::ContainsPoint( iPoint );
}


bool
cHUDTransform::Event( QEvent * iEvent )
{
    // Do all event stuff here, so we don't have to then send a message from handle to say handleMoved(), then catch it here, then make the changes, then change handle positions,
    // then keep tracking the mouse during all that on the handle ......
    QMouseEvent* eventAsMouse = 0;

    switch( iEvent->type() )
    {
        case QEvent::MouseButtonPress :
            eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );
            mFocusedHandle = _GetHandleAtPoint( ApplyInvertTransformationComposition( eventAsMouse->pos() ) );
            if( mFocusedHandle )
            {
                qDebug() << "HandlePress";
            }
            else
            {
                qDebug() << "TransformPress";
            }
            return  true;

        case QEvent::MouseMove :
            if( mFocusedHandle )
            {
                qDebug() << "HandleMove";
            }
            else
            {
                qDebug() << "TransformMove";
            }
            return  true;

        case QEvent::MouseButtonRelease:
            if( mFocusedHandle )
            {
                qDebug() << "HandleRelease";
            }
            else
            {
                qDebug() << "TransformRelease";
            }
            return  true;

        default:
            return  false;
    }

    return false;
}


void
cHUDTransform::_LayoutChildren()
{
    QRect frame( -mHandleSize/2, -mHandleSize/2, mHandleSize, mHandleSize );

    auto topLeft = mChildrenHUDs[ 0 ];
    topLeft->SetFrame( frame );

    frame.translate( QPoint( mOriginalFrame.width(), 0 ) );
    auto topRight = mChildrenHUDs[ 1 ];
    topRight->SetFrame( frame );

    frame.translate( QPoint( 0, mOriginalFrame.height() ) );
    auto botRight = mChildrenHUDs[ 2 ];
    botRight->SetFrame( frame );

    frame.translate( QPoint( -mOriginalFrame.width(), 0 ) );
    auto botLeft = mChildrenHUDs[ 3 ];
    botLeft->SetFrame( frame );
}


cHUDHandle*
cHUDTransform::_GetHandleAtPoint( const QPoint & iPoint )
{
    for( auto handle : mChildrenHUDs )
    {
        if( handle->ContainsPoint( iPoint ) )
            return  dynamic_cast< cHUDHandle* >( handle );
    }

    return  nullptr;
}

