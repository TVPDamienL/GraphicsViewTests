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


QTransform
cHUDView::GetTransform()
{
    return  mTransform;
}


void
cHUDView::TranslateBy( const QPointF & iOffset )
{
    mTranslation += iOffset;
    _RecomputeMatrix();
    update();
}


void
cHUDView::ScaleBy( double iScale )
{
    mScale *= iScale;
    _RecomputeMatrix();
    update();
}



const QPointF&
cHUDView::Translation() const
{
    return  mTranslation;
}

double
cHUDView::Scale() const
{
    return  mScale;
}


double
cHUDView::RotationAngle() const
{
    return  mRotationAngle;
}


void
cHUDView::SetTranslation( const QPointF & iTranslation )
{
    mTranslation = iTranslation;
    _RecomputeMatrix();
}


void
cHUDView::SetScale( double iScale )
{
    mScale = iScale;
    _RecomputeMatrix();
}


void
cHUDView::SetRotation( double iAngle )
{
    mRotationAngle = iAngle;
    _mCosAngle = cos( iAngle );
    _mSinAngle = sin( iAngle );
    _RecomputeMatrix();
}


void
cHUDView::AddHUDObject( cHUDObject * iObject )
{
    mHUDObjects.push_back( iObject );
}


cHUDObject*
cHUDView::GetVisibleHUDObjectAtPos( const QPointF & iPoint )
{
    for( auto hud : mHUDObjects )
    {
        auto hudObj = hud->GetVisibleHUDObjectAtPos( iPoint );
            if( hudObj )
                return  hudObj;
    }

    return  0;
}


void
cHUDView::_RecomputeMatrix()
{
    const double cosScaled = _mCosAngle * mScale;
    const double sinScaled = _mSinAngle * mScale;

    mTransform.setMatrix( cosScaled, sinScaled, 0, -sinScaled, cosScaled, 0, mTranslation.x(), mTranslation.y(), 1 );
}
