#include "cHUDTransform.h"

#include "cSelection.h"

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
    iPainter->drawRect( ToHUDCoords( mOriginalFrame ) );
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
            mOriginGlobalXScale = GlobalXScale();
            mOriginGlobalYScale = GlobalYScale();

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

            if( mFocusedHandle )
            {
                QPointF offset = (eventAsMouse->pos() - mClickOrigin);
                offset.setX( offset.x() / mOriginGlobalXScale );
                offset.setY( offset.y() / mOriginGlobalYScale );

                auto currentHandleVector = mFocusedHandle->GetFrame().topLeft() + offset - mFocusedHandleOpposite->GetFrame().topLeft();

                double xScale = currentHandleVector.x() / mOriginHandleVector.x();
                double yScale = currentHandleVector.y() / mOriginHandleVector.y();

                mTranslation = mOriginTranslation;
                mXScale = mOriginXScale;
                mYScale = mOriginYScale;

                CenterScale( mFocusedHandleOpposite->GetFrame().center(), xScale, yScale );
            }
            else
            {
                QPointF offset = eventAsMouse->pos() - mClickOrigin;
                mTranslation = mOriginTranslation + offset / mParentView->Scale();
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


