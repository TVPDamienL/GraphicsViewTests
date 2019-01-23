#include "cHUDTransform.h"


cHUDTransform::~cHUDTransform()
{
}


cHUDTransform::cHUDTransform()
{
    auto topLeft = new cHUDHandle();
    mChildrenHUDs.push_back( topLeft );

    auto topRight = new cHUDHandle();
    mChildrenHUDs.push_back( topRight );

    auto botRight = new cHUDHandle();
    mChildrenHUDs.push_back( botRight );

    auto botLeft = new cHUDHandle();
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
    iPainter->drawRect( mFrame );
}


void
cHUDTransform::SetFrame( const QRect & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    _LayoutChildren();

}


void
cHUDTransform::MoveBy( const QPoint & iOffset )
{
    cHUDObject::MoveBy( iOffset );
    _LayoutChildren();
}


void
cHUDTransform::ScaleBy( float iScale )
{
    cHUDObject::ScaleBy( iScale );
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
        case QEvent::MouseMove :
        case QEvent::MouseButtonRelease:
            eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );
            if( ContainsPoint( eventAsMouse->pos() ) )
            {
                qDebug() << "TransformEvent";
            }

        default:
            break;
    }

    return false;
}


void
cHUDTransform::_LayoutChildren()
{
    QRect frame( mFrame.left() - mHandleSize/2, mFrame.top()  - mHandleSize/2, mHandleSize, mHandleSize );

    auto topLeft = mChildrenHUDs[ 0 ];
    topLeft->SetFrame( frame );

    frame.translate( QPoint( mFrame.width(), 0 ) );
    auto topRight = mChildrenHUDs[ 1 ];
    topRight->SetFrame( frame );

    frame.translate( QPoint( 0, mFrame.height() ) );
    auto botRight = mChildrenHUDs[ 2 ];
    botRight->SetFrame( frame );

    frame.translate( QPoint( -mFrame.width(), 0 ) );
    auto botLeft = mChildrenHUDs[ 3 ];
    botLeft->SetFrame( frame );
}

