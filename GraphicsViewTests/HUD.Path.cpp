#include "HUD.Path.h"


cHUDPath::~cHUDPath()
{
}


cHUDPath::cHUDPath( cHUDView* iParentView, cHUDObject* iParentObject, const std::vector< sPointData >& iPath ) :
    cHUDObject( iParentView, iParentObject )
{
    mPath = iPath;
    for( auto& point : iPath )
    {
		mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
	}

    mVisible = false;

    _LayoutChildren();
}


void
cHUDPath::Draw( QPainter * iPainter )
{
    for( auto handle : mChildrenHUDs )
    {
        handle->Draw( iPainter );
    }
}


void
cHUDPath::SetFrame( const QRectF & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    _LayoutChildren();

}


bool
cHUDPath::ContainsPoint( const QPointF & iPoint ) const
{
    for( auto handle : mChildrenHUDs )
    {
        if( handle->ContainsPoint( iPoint ) )
            return  true;
    }

    return  cHUDObject::ContainsPoint( iPoint );
}


bool
cHUDPath::Event( QEvent * iEvent )
{
    // Do all event stuff here, so we don't have to then send a message from handle to say handleMoved(), then catch it here, then make the changes, then change handle positions,
    // then keep tracking the mouse during all that on the handle ......
    QMouseEvent* eventAsMouse = 0;

    switch( iEvent->type() )
    {
        case QEvent::MouseButtonPress :
            return  true;

        case QEvent::MouseMove :
            return  true;

        case QEvent::MouseButtonRelease:
            return  true;

        default:
            return  false;
    }

    return false;
}


void
cHUDPath::SetHandleColor( const QColor & color )
{
    for( int i = 0; i < mChildrenHUDs.size(); ++i )
    {
        auto child = dynamic_cast< cHUDHandle* >( mChildrenHUDs[ i ] );
        child->mColor = color;
    }
}


void
cHUDPath::_LayoutChildren()
{
    for( int i = 0; i<mChildrenHUDs.size(); ++i )
    {
		sPointData& point = mPath[ i ];
		auto child = mChildrenHUDs[ i ];

		QRect frame( point.mPosition.x() -HANDLESIZE/2, point.mPosition.y() -HANDLESIZE/2, HANDLESIZE, HANDLESIZE );
        child->SetFrame( frame );
    }
}


cHUDHandle*
cHUDPath::_GetHandleAtPoint( int* oIndex, const QPointF & iPoint )
{
    *oIndex = -1;
    for( auto handle : mChildrenHUDs )
    {
        ++(*oIndex);
        if( handle->ContainsPoint( iPoint ) )
            return  dynamic_cast< cHUDHandle* >( handle );
    }

    *oIndex = -1;
    return  nullptr;
}