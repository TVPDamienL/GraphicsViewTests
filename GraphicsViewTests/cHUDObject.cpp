#include "cHUDObject.h"

#include <QEvent>

cHUDObject::~cHUDObject()
{
}


cHUDObject::cHUDObject()
{
    mTransformation.setToIdentity();
}


void
cHUDObject::SetFrame( const QRect & iFrame )
{
    mOriginalFrame = iFrame;
    mFrame = MappedRect( mOriginalFrame );
}


void
cHUDObject::MoveBy( const QPoint & iOffset )
{
    mTransformation( 0, 2 ) += iOffset.x();
    mTransformation( 1, 2 ) += iOffset.y();

    mFrame = MappedRect( mOriginalFrame );
}


void
cHUDObject::ScaleBy( float iScale )
{
    mTransformation( 0, 0 ) *= iScale;
    mTransformation( 1, 1 ) *= iScale;

    mFrame = MappedRect( mOriginalFrame );
}


bool
cHUDObject::Event( QEvent * iEvent )
{
    QMouseEvent* eventAsMouse = 0;
    switch( iEvent->type() )
    {
        case QEvent::MouseButtonPress :
        case QEvent::MouseMove :
        case QEvent::MouseButtonRelease:
            eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );
            if( ContainsPoint( eventAsMouse->pos() ) )
            {
                for( auto child : mChildrenHUDs )
                {
                    if( child->Event( iEvent ) ) // If event has been handled, return true, which means handled
                        return  true;
                }
            }

        default:
            break;
    }

    return  false;
}


QRect
cHUDObject::MappedRect( const QRect & iRect )
{
    QRect output;

    output.setTopLeft( MappedPoint( iRect.topLeft() ) );
    output.setBottomRight( MappedPoint( iRect.bottomRight() ) );

    return  output;
}


QPoint
cHUDObject::MappedPoint( const QPoint & iPoint )
{
    QGenericMatrix< 1, 3, float > pointMatrix;
    pointMatrix( 0, 0 ) = iPoint.x();
    pointMatrix( 1, 0 ) = iPoint.y();
    pointMatrix( 2, 0 ) = 1;

    auto transfoResult = mTransformation * pointMatrix;
    return  QPoint( transfoResult( 0, 0 ), transfoResult( 1, 0 ) );
}


bool
cHUDObject::ContainsPoint( const QPoint & iPoint ) const
{
    return  mFrame.contains( iPoint );
}

