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
    //auto transform = new cHUDTransform( this, 0 );
    //transform->SetFrame( QRect( 0, 0, 100, 100 ) );
    //mHUDObjects.push_back( transform );
    mGlobalTransformation.reset();
}


void
cHUDView::paintEvent( QPaintEvent * iEvent )
{
    QPainter painter( this );
    for( auto hud : mHUDObjects )
    {
        if( hud->Visible() )
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
    mPressedHUD = GetVisibleHUDObjectAtPos( iEvent->pos() );

    while( mPressedHUD )
    {
        if( mPressedHUD->Event( iEvent ) )
            return;

        mPressedHUD = mPressedHUD->ParentObject();
    }

    iEvent->ignore();
    QWidget::mousePressEvent( iEvent );
}


void
cHUDView::mouseMoveEvent( QMouseEvent * iEvent )
{
    if( mPressedHUD && mPressedHUD->Event( iEvent ) )
        return;

    iEvent->ignore();
    QWidget::mouseMoveEvent( iEvent );
}


void
cHUDView::mouseReleaseEvent( QMouseEvent * iEvent )
{
    if( mPressedHUD && mPressedHUD->Event( iEvent ) )
        return;

    iEvent->ignore();
    QWidget::mouseReleaseEvent( iEvent );
}


void
cHUDView::wheelEvent( QWheelEvent * iEvent )
{
    iEvent->ignore();
    QWidget::wheelEvent( iEvent );
}


QTransform&
cHUDView::GetTransform()
{
    return  mGlobalTransformation;
}


void
cHUDView::TranslateBy( const QPoint & iOffset )
{
    const float scaleInverse = 1/mScale;

    // Transformation goes from latest to first ( reverse order )
    mGlobalTransformation.scale( scaleInverse, scaleInverse );      // Then we unscale, so that offset is scaled properly
    // Probably needs a Unrotate as well
    mGlobalTransformation.translate( iOffset.x(), iOffset.y() );    // Then we translate
    // Probably needs a Rerotate as well
    mGlobalTransformation.scale( mScale, mScale );                  // First we scale

    update();
}


void
cHUDView::ScaleBy( float iScale )
{
    mScale *= iScale;

    // This will put scale in first place, which is fine, as scaling is usually the first thing to be done
    mGlobalTransformation.scale( iScale, iScale );

    update();
}


void
cHUDView::SetDrawingAreaOffset( const QPoint & iOffset )
{
    mGlobalTransformation.translate( iOffset.x(), iOffset.y() );

    mDrawingAreaOffset = iOffset;
}


float
cHUDView::Scale() const
{
    return  mScale;
}


void
cHUDView::AddHUDObject( cHUDObject * iObject )
{
    mHUDObjects.push_back( iObject );
}


cHUDObject*
cHUDView::GetVisibleHUDObjectAtPos( const QPoint & iPoint )
{
    for( auto hud : mHUDObjects )
    {
        auto hudObj = hud->GetVisibleHUDObjectAtPos( iPoint );
            if( hudObj )
                return  hudObj;
    }

    return  0;
}


QPoint
cHUDView::MapToView( const QPoint & iPoint )
{
    return  mGlobalTransformation.map( iPoint );
}


QRect
cHUDView::MapToView( const QRect & iRect )
{
    return  mGlobalTransformation.mapRect( iRect );
}
