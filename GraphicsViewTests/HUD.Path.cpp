#include "HUD.Path.h"


cHUDPath::~cHUDPath()
{
}


cHUDPath::cHUDPath( cHUDView* iParentView, cHUDObject* iParentObject, const std::vector< sPointData >& iPath, cPaintToolBase* tool, cClip* clip ) :
    cHUDObject( iParentView, iParentObject )
{
    mPath = iPath;
    mTool = tool;
    mClip = clip;


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

    if( iEvent->type() == QEvent::MouseButtonPress )
    {
        eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );
        mOriginClickPos = eventAsMouse->pos();
        mOriginHandle = _GetHandleAtPoint( &mHandleIndex, eventAsMouse->pos() );
        mOriginHandleFrame = mOriginHandle->GetFrame();
        return  true;
    }
    else if( iEvent->type() == QEvent::MouseMove )
    {
        eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );

        QPointF offset = FromHUDCoords( eventAsMouse->pos() ) - FromHUDCoords( mOriginClickPos );
        QRectF frame = mOriginHandleFrame;
        frame.moveCenter( frame.center() + offset );
        mOriginHandle->SetFrame( frame );

        auto center = frame.center();

        sPointData p;
        p.mPosition = QPoint( center.x(), center.y() );
        p.mPressure = 1.0;
        p.mRotation = 0.0;

        mTool->PathSetPoint( mHandleIndex, p );
        mTool->CancelDrawing();
        mTool->DrawFullPath();
        mClip->DirtyAll();

        return  true;
    }
    else if( iEvent->type() == QEvent::MouseButtonRelease )
    {
        return  true;
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