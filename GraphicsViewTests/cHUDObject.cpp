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
    mXScale = 1.0;
    mYScale = 1.0;
    mRotationAngle = 0.0;
}


void
cHUDObject::MoveBy( const QPointF & iOffset )
{
    mTranslation += iOffset;
    mParentView->update();
}


void
cHUDObject::ScaleBy( double iXScale, double iYScale )
{
    mXScale *= iXScale;
    mYScale *= iYScale;
    mParentView->update();
}


void
cHUDObject::CenterScale( const QPointF & iCenter, double iXScale, double iYScale )
{
    mTranslation.setX( mXScale * ( iCenter.x() - iXScale * iCenter.x() ) + mTranslation.x() );
    mTranslation.setY( mYScale * ( iCenter.y() - iYScale * iCenter.y() ) + mTranslation.y() );
    ScaleBy( iXScale, iYScale );
}


double
cHUDObject::LocalXScale() const
{
    return  mXScale;
}


double
cHUDObject::LocalYScale() const
{
    return  mYScale;
}


double
cHUDObject::GlobalXScale() const
{
    double scale = mParentView->Scale();
    if( mParentObject )
        scale = mParentObject->GlobalXScale();

    return  mXScale * scale;
}


double
cHUDObject::GlobalYScale() const
{
    double scale = mParentView->Scale();
    if( mParentObject )
        scale = mParentObject->GlobalYScale();

    return  mYScale * scale;
}


QPointF
cHUDObject::GlobalTranslation() const
{
    QPointF trans = mParentView->Translation();
    if( mParentObject )
        trans = mParentObject->GlobalTranslation();

    return  QPointF( mXScale * trans.x() + mTranslation.x(), mYScale * trans.y() + mTranslation.y() );
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


    // QTransform probably is optimized enough to not need to do that
    //auto finalTranslation = GlobalTranslation();
    //auto finalScale = GlobalScale();
    //// auto finalRotation = GlobalAngle();
    //
    //return  QTransform::fromScale( finalScale, finalScale ) * QTransform::fromTranslate( finalTranslation.x(), finalTranslation.y() );
}


QTransform
cHUDObject::GetLocalTransform() const
{
    return QTransform::fromScale( mXScale, mYScale ) * QTransform::fromTranslate( mTranslation.x(), mTranslation.y() );
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

