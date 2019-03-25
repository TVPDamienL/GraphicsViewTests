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
		mChildrenHUDs.push_back( new cHUDHandlePath( iParentView, this ) );
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
        mOriginHandle = GetObjectAtPoint( eventAsMouse->pos() );
        auto objectAsHandleHUD = dynamic_cast< cHUDHandlePath* >( mOriginHandle );
        auto objectAsHandle = dynamic_cast< cHUDHandle* >( mOriginHandle );

        if( objectAsHandleHUD )
            mState = kDragPosition;
        else if( objectAsHandle )
            mState = kDragPressure;

        mOriginHandleFrame = mOriginHandle->GetFrame();
        mHandleIndex = _GetHandleIndex( mOriginHandle );

        qDebug() << "T";

        return  true;
    }
    else if( iEvent->type() == QEvent::MouseMove )
    {
        eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );

        if( mState == kDragPosition )
        {
            QPointF offset = FromHUDCoords( eventAsMouse->pos() ) - FromHUDCoords( mOriginClickPos );
            QRectF frame = mOriginHandleFrame;
            frame.moveCenter( frame.center() + offset );
            mOriginHandle->SetFrame( frame );

            auto center = frame.center();

            sPointData p = mTool->PathGetPoint( mHandleIndex );
            p.mPosition = QPoint( center.x(), center.y() );

            mTool->PathSetPoint( mHandleIndex, p );
            mTool->CancelDrawing();
            mTool->DrawFullPath();
            mClip->DirtyAll();
        }
        else if( mState == kDragPressure )
        {
            qDebug() << "Pressure";
        }

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
		auto child = dynamic_cast< cHUDHandlePath* >( mChildrenHUDs[ i ] );

		QRect frame( point.mPosition.x() -HANDLESIZE/2, point.mPosition.y() -HANDLESIZE/2, HANDLESIZE, HANDLESIZE );
        child->SetFrame( frame );
        child->setSize( ( mTool->getSize() * point.mPressure * 2 ) + 1 );
    }
}


cHUDHandlePath*
cHUDPath::_GetHandleAtPoint( int* oIndex, const QPointF & iPoint )
{
    *oIndex = -1;
    for( auto handle : mChildrenHUDs )
    {
        ++(*oIndex);
        if( handle->ContainsPoint( iPoint ) )
            return  dynamic_cast< cHUDHandlePath* >( handle );
    }

    *oIndex = -1;
    return  nullptr;
}


int
cHUDPath::_GetHandleIndex( const cHUDObject* iObject )
{
    for( int i = 0; i < mChildrenHUDs.size(); ++i )
    {
        auto child = dynamic_cast< cHUDHandlePath* >( mChildrenHUDs[ i ] );
        if( child == iObject || iObject == child->GetHandlePtr() )
            return  i;
    }

    return 0;
}
