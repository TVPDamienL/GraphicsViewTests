#include "cHUDView.h"

#include "cHUDTransform.h"

#include <QEvent>
#include <QDebug>
#include <QTabletEvent>

#include "Canvas.h"


cHUDView::~cHUDView()
{
}


cHUDView::cHUDView( QWidget* iParent ) :
    QWidget( iParent )
{
    auto transform = new cHUDTransform();
    transform->SetFrame( QRect( 0, 0, 100, 100 ) );
    mHUDObjects.push_back( transform );
}


void
cHUDView::paintEvent( QPaintEvent * iEvent )
{
    QPainter painter( this );
    for( auto hud : mHUDObjects )
    {
        hud->Draw( &painter );
    }
}


void
cHUDView::tabletEvent( QTabletEvent * iEvent )
{
    iEvent->ignore();
    QWidget::tabletEvent( iEvent );
}


void
cHUDView::dragEnterEvent( QDragEnterEvent * iEvent )
{
    iEvent->ignore();
    QWidget::dragEnterEvent( iEvent );
}


void
cHUDView::dragMoveEvent( QDragMoveEvent * iEvent )
{
    iEvent->ignore();
    QWidget::dragMoveEvent( iEvent );
}


void
cHUDView::dragLeaveEvent( QDragLeaveEvent * iEvent )
{
    iEvent->ignore();
    QWidget::dragLeaveEvent( iEvent );
}


void
cHUDView::dropEvent( QDropEvent * iEvent )
{
    iEvent->ignore();
    QWidget::dropEvent( iEvent );
}


void
cHUDView::keyPressEvent( QKeyEvent * iEvent )
{
    iEvent->ignore();
    QWidget::keyPressEvent( iEvent );
}


void
cHUDView::keyReleaseEvent( QKeyEvent * iEvent )
{
    iEvent->ignore();
    QWidget::keyReleaseEvent( iEvent );
}


void
cHUDView::mousePressEvent( QMouseEvent * iEvent )
{
    for( auto hud : mHUDObjects )
    {
        if( hud->Event( iEvent ) )
            return;
    }

    iEvent->ignore();
    QWidget::mousePressEvent( iEvent );
}


void
cHUDView::mouseMoveEvent( QMouseEvent * iEvent )
{
    for( auto hud : mHUDObjects )
    {
        if( hud->Event( iEvent ) )
            return;
    }

    iEvent->ignore();
    QWidget::mouseMoveEvent( iEvent );
}


void
cHUDView::mouseReleaseEvent( QMouseEvent * iEvent )
{
    for( auto hud : mHUDObjects )
    {
        if( hud->Event( iEvent ) )
            return;
    }

    iEvent->ignore();
    QWidget::mouseReleaseEvent( iEvent );
}


void
cHUDView::wheelEvent( QWheelEvent * iEvent )
{
    iEvent->ignore();
    QWidget::wheelEvent( iEvent );
}


void
cHUDView::ApplyPan( const QPoint & iOffset )
{
    for( auto hud : mHUDObjects )
    {
        hud->MoveBy( iOffset );
    }

    update();
}


void
cHUDView::ApplyZoom( float iScale )
{
    mScale = iScale;

    for( auto hud : mHUDObjects )
    {
        hud->ScaleBy( iScale );
    }

    update();
}


void
cHUDView::SetDrawingAreaOffset( const QPoint & iOffset )
{
    for( auto hud : mHUDObjects )
    {
        hud->MoveBy( -mDrawingAreaOffset + iOffset ); // Remove old offset, add new one, to update the drawingarea offset
    }

    mDrawingAreaOffset = iOffset;
}


float
cHUDView::Scale() const
{
    return  mScale;
}


