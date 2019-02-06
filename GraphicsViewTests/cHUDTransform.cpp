#include "cHUDTransform.h"

#include "cSelection.h"

#include "cHUDView.h"


cHUDTransform::~cHUDTransform()
{
}


cHUDTransform::cHUDTransform( cHUDView* iParentView, cHUDObject* iParentObject ) :
    cHUDObject( iParentView, iParentObject )
{
    mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
    mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
    mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
    mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
    mPivot = new cHUDHandle( iParentView, this );
    mChildrenHUDs.push_back( mPivot );

    mVisible = false;

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
    iPainter->drawPolygon( ToHUDCoords( mOriginalFrame ) );
}


void
cHUDTransform::SetFrame( const QRectF & iFrame )
{
    cHUDObject::SetFrame( iFrame );
    _LayoutChildren();

}


bool
cHUDTransform::ContainsPoint( const QPointF & iPoint ) const
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
            mClickOrigin = eventAsMouse->pos();
            mOriginTranslation = mTranslation;
            mOriginXScale = mXScale;
            mOriginYScale = mYScale;
            mOriginRotation = mRotationAngle;
            mOriginTransformInverse = GetFinalTransform().inverted();
            mOriginLocalTransform = GetLocalTransform();

            int index;
            mFocusedHandle          = _GetHandleAtPoint( &index, eventAsMouse->pos() );
            mFocusedHandleOpposite  = _GetOppositeHandle( index );

            if( mFocusedHandle && mFocusedHandleOpposite )
            {
                mOriginHandleVector = mFocusedHandle->GetFrame().topLeft() - mFocusedHandleOpposite->GetFrame().topLeft();
            }

            return  true;

        case QEvent::MouseMove :
            eventAsMouse = dynamic_cast< QMouseEvent* >( iEvent );

            if( mFocusedHandle && mFocusedHandle != mPivot )
            {
                QPointF offset = ( mOriginTransformInverse.map( eventAsMouse->pos() ) - mOriginTransformInverse.map( mClickOrigin) );

                auto currentHandleVector = mFocusedHandle->GetFrame().topLeft() + offset - mFocusedHandleOpposite->GetFrame().topLeft();

                double xScale = currentHandleVector.x() / mOriginHandleVector.x();
                double yScale = currentHandleVector.y() / mOriginHandleVector.y();

                mTranslation = mOriginTranslation;
                mXScale = mOriginXScale;
                mYScale = mOriginYScale;

                CenterScale( mFocusedHandleOpposite->GetFrame().center(), xScale, yScale );
            }
            else if( mFocusedHandle == mPivot )
            {
                auto clickMapped    = mOriginTransformInverse.map( mClickOrigin );
                auto centerMapper   = mOriginalFrame.center();
                auto currentMapped  = mOriginTransformInverse.map( eventAsMouse->pos() );

                QPointF originVector = clickMapped - centerMapper;
                QPointF newVector = currentMapped - centerMapper;

                double firstAngle = atan2( originVector.y(), originVector.x() );
                double secondAngle = atan2( newVector.y(), newVector.x() );

                double theAngle = secondAngle - firstAngle;

                mTranslation = mOriginTranslation;
                mRotationAngle = mOriginRotation;
                _mCosAngle = cos( mOriginRotation );
                _mSinAngle = sin( mOriginRotation );

                //CenterRotation( mOriginalFrame.center(), 20 * PI/180 );
                CenterRotationPostTransform( mOriginLocalTransform.map( mOriginalFrame.center() ), theAngle );
                //const auto point = mOriginalFrame.center();
                //const auto point = QPointF( 250, 250 );
                //CenterRotation( point, theAngle );

                //qDebug() << "Angle : " << theAngle * 180/PI;
                //qDebug() << "Pt : " << point;
            }
            else
            {
                QPointF offset = eventAsMouse->pos() - mClickOrigin;
                mTranslation = mOriginTranslation + offset / mParentView->Scale();
            }

            mSelection->TransformSelection( _GetLocalTransformWithTopLeftOriginNoScale(), mXScale, mYScale );

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
cHUDTransform::SetSelection( cSelection * iSelection )
{
    mSelection = iSelection;
    mSelection->RegisterEditionCallback( [ this ]( cBaseData* iSender, int iArg ){
        this->SelectionChangedEvent( iSender, iArg );
    });
}


void
cHUDTransform::SelectionChangedEvent( cBaseData * iSender, int iArg )
{
    if( iArg == cSelection::eMessageSelection::kActiveChanged )
    {
        Visible( mSelection->IsActive() );
        SetFrame( mSelection->GetSelectionBBox() );
        ResetTransformation();
    }
    else if( iArg == cSelection::eMessageSelection::kBoundsChanged )
    {
        SetFrame( mSelection->GetSelectionBBox() );
    }
}


void
cHUDTransform::_LayoutChildren()
{
    QRect frame( mOriginalFrame.left() -HANDLESIZE/2, mOriginalFrame.top() -HANDLESIZE/2, HANDLESIZE, HANDLESIZE );

    auto topLeft = mChildrenHUDs[ 0 ];
    topLeft->SetFrame( frame );

    frame.translate( mOriginalFrame.width() / 2, 0 );
    mPivot->SetFrame( frame );

    frame.translate( mOriginalFrame.width() / 2, 0 );
    auto topRight = mChildrenHUDs[ 1 ];
    topRight->SetFrame( frame );

    frame.translate( QPoint( 0, mOriginalFrame.height() ) );
    auto botRight = mChildrenHUDs[ 2 ];
    botRight->SetFrame( frame );

    frame.translate( QPoint( -mOriginalFrame.width(), 0 ) );
    auto botLeft = mChildrenHUDs[ 3 ];
    botLeft->SetFrame( frame );


    mPolygon.clear();
    mPolygon.append( QPointF( 0, 0 ) );
    mPolygon.append( QPointF( 10, 0 ) );
    mPolygon.append( QPointF( 10, 10 ) );
    mPolygon.append( QPointF( 0, 10 ) );
    mPolygon.translate( mOriginalFrame.center() );
}


cHUDHandle*
cHUDTransform::_GetHandleAtPoint( int* oIndex, const QPointF & iPoint )
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


cHUDHandle*
cHUDTransform::_GetOppositeHandle( int iIndex )
{
    if( iIndex < 0 )
        return  0;

    return  dynamic_cast< cHUDHandle* >( mChildrenHUDs[ (iIndex + 2) % 4 ] );
}


QTransform
cHUDTransform::_GetLocalTransformWithTopLeftOriginNoScale() const
{
    auto localTransfor = GetLocalTransform();
    QPointF topLeft = mOriginalFrame.topLeft();
    QPointF topLeftLocalMap = localTransfor.map( topLeft );
    QPointF diff = topLeftLocalMap - topLeft;
    double cosAngle = 1; // cos( mAngle )
    double sinAngle = 0; // sin( mAngle )

    localTransfor.setMatrix( cosAngle, sinAngle, 0, -sinAngle, cosAngle, 0, diff.x(), diff.y(), 1 );
    return  localTransfor;
}


