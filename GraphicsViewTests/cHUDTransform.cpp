#include "cHUDTransform.h"


cHUDTransform::~cHUDTransform()
{
}


cHUDTransform::cHUDTransform()
{
    auto topLeft = new cHUDHandle();
    mHandles.push_back( topLeft );

    auto topRight = new cHUDHandle();
    mHandles.push_back( topRight );

    auto botRight = new cHUDHandle();
    mHandles.push_back( botRight );

    auto botLeft = new cHUDHandle();
    mHandles.push_back( botLeft );

    _LayoutChildren();
}


void
cHUDTransform::Draw( QPainter * iPainter )
{
    for( auto handle : mHandles )
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
cHUDTransform::_LayoutChildren()
{
    QRect frame( mFrame.left() - mHandleSize/2, mFrame.top()  - mHandleSize/2, mHandleSize, mHandleSize );

    auto topLeft = mHandles[ 0 ];
    topLeft->SetFrame( frame );

    frame.translate( QPoint( mFrame.width(), 0 ) );
    auto topRight = mHandles[ 1 ];
    topRight->SetFrame( frame );

    frame.translate( QPoint( 0, mFrame.height() ) );
    auto botRight = mHandles[ 2 ];
    botRight->SetFrame( frame );

    frame.translate( QPoint( -mFrame.width(), 0 ) );
    auto botLeft = mHandles[ 3 ];
    botLeft->SetFrame( frame );
}

