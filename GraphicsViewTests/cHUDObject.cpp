#include "cHUDObject.h"

#include "cHUDView.h"

#include <QEvent>

cHUDObject::~cHUDObject()
{
}


cHUDObject::cHUDObject( cHUDView* iParentView, cHUDObject* iParentObject )
{
    mParentView = iParentView;
    mParentObject = iParentObject;
}


cHUDObject*
cHUDObject::ParentObject()
{
    return  mParentObject;
}


void
cHUDObject::SetFrame( const QRectF & iFrame )
{
    mOriginalFrame = iFrame;
}


QRectF
cHUDObject::GetFrame() const
{
    return  mOriginalFrame;
}


void
cHUDObject::ResetTransformation()
{
    mTranslation = QPointF( 0, 0 );
    mScale = 1.0;
    mRotationAngle = 0.0;
}


void
cHUDObject::MoveBy( const QPointF & iOffset )
{
    mTranslation += iOffset;
    mParentView->update();
}


void
cHUDObject::ScaleBy( double iScale )
{
    mScale *= iScale;
    mParentView->update();
}


void
cHUDObject::CenterScale( const QPointF & iCenter, double iScale )
{
    mTranslation.setX( mScale * ( iCenter.x() - iScale * iCenter.x() ) + mTranslation.x() );
    mTranslation.setY( mScale * ( iCenter.y() - iScale * iCenter.y() ) + mTranslation.y() );
    mScale *= iScale;
}


double
cHUDObject::Scale() const
{
    return  mScale;
}


bool
cHUDObject::Event( QEvent * iEvent )
{
    for( auto child : mChildrenHUDs )
    {
        if( child->Event( iEvent ) ) // If event has been handled, return true, which means handled
            return  true;
    }

    return  false;
}


bool
cHUDObject::Visible() const
{
    return  mVisible;
}


void
cHUDObject::Visible( bool iVisible )
{
    mVisible = iVisible;
}


QRectF
cHUDObject::ToHUDCoords( const QRectF & iRect )
{
    return  GetFinalTransform().mapRect( iRect );
}


QPointF
cHUDObject::ToHUDCoords( const QPointF & iPoint )
{
    return  GetFinalTransform().map( iPoint );
}


QPointF
cHUDObject::FromHUDCoords( const QPointF& iPoint ) const
{
    return  GetFinalTransform().inverted().map( iPoint );
}


QTransform
cHUDObject::GetFinalTransform() const
{
    QTransform parentsTransforms = mParentView->GetTransform();

    if( mParentObject )
        parentsTransforms = mParentObject->GetFinalTransform();

    return  GetLocalTransform() * parentsTransforms;
}


QTransform
cHUDObject::GetLocalTransform() const
{
    return QTransform::fromScale( mScale, mScale ) * QTransform::fromTranslate( mTranslation.x(), mTranslation.y() );
}


bool
cHUDObject::ContainsPoint( const QPointF & iPoint ) const
{
    QPointF mappedPoint = FromHUDCoords( iPoint );
    return  mOriginalFrame.contains( mappedPoint );
}


cHUDObject*
cHUDObject::GetVisibleHUDObjectAtPos( const QPointF & iPoint )
{
    // Deepest child first
    for( auto child : mChildrenHUDs )
        if( child->Visible() && child->ContainsPoint( iPoint ) )
            return  child;

    // Then, if no child, myself
    if( Visible() && mOriginalFrame.contains( FromHUDCoords( iPoint ) ) )
        return  this;

    // If i'm not in, nothing
    return  0;
}

