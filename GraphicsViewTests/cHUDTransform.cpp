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
            mFocusedHandle = _GetHandleAtPoint( eventAsMouse->pos() );
            if( mFocusedHandle )
            {
                qDebug() << "HandlePress";
            }
            else
            {
                auto mouseEvent = dynamic_cast< QMouseEvent* >( iEvent );
                mClickOrigin = mouseEvent->pos();
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
                auto mouseEvent = dynamic_cast< QMouseEvent* >( iEvent );
                QPoint offset = mouseEvent->pos() - mClickOrigin;
                MoveBy( offset / mParentView->Scale() );
                mClickOrigin = mouseEvent->pos();
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
cHUDTransform::_GetHandleAtPoint( const QPointF & iPoint )
{
    for( auto handle : mChildrenHUDs )
    {
        if( handle->ContainsPoint( iPoint ) )
            return  dynamic_cast< cHUDHandle* >( handle );
    }

    return  nullptr;
}

