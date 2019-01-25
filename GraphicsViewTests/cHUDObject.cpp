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
cHUDObject::SetFrame( const QRect & iFrame )
{
    mOriginalFrame = iFrame;
}


QRect
cHUDObject::GetFrame() const
{
    return  mOriginalFrame;
}


void
cHUDObject::MoveBy( const QPoint & iOffset )
{
    const float scaleInverse = 1/mScale;

    mObjectSelfTransformation.scale( scaleInverse, scaleInverse );      // Unscale
    // Probably needs a Unrotate as well
    mObjectSelfTransformation.translate( iOffset.x(), iOffset.y() );    // Translate
    // Probably needs a Rerotate as well
    mObjectSelfTransformation.scale( mScale, mScale );                  // Rescale

    mParentView->update();
}


void
cHUDObject::ScaleBy( float iScale )
{
    mScale *= iScale;
    mObjectSelfTransformation.scale( iScale, iScale );

    mParentView->update();
}


float
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


QTransform*
cHUDObject::GetTransform()
{
    return  &mObjectSelfTransformation;
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


QRect
cHUDObject::MapToObject( const QRect & iRect )
{
    return  mObjectSelfTransformation.mapRect( iRect );
}


QPoint
cHUDObject::MapToObject( const QPoint & iPoint )
{
    return  mObjectSelfTransformation.map( iPoint );
}


QPoint
cHUDObject::ApplyInvertTransformationComposition( const QPoint& iPoint ) const
{
    QTransform composedTransformationInverse = mParentView->GetTransform() * mObjectSelfTransformation;
    composedTransformationInverse = composedTransformationInverse.inverted();
    return  composedTransformationInverse.map( iPoint );
}


QPoint
cHUDObject::ApplyTransformationComposition( const QPoint & iPoint ) const
{
    QTransform composedTransformation = mParentView->GetTransform() * mObjectSelfTransformation;
    return  composedTransformation.map( iPoint );
}


bool
cHUDObject::ContainsPoint( const QPoint & iPoint ) const
{
    QPoint mappedPoint = ApplyInvertTransformationComposition(  iPoint );
    return  mOriginalFrame.contains( iPoint );
}


cHUDObject*
cHUDObject::GetVisibleHUDObjectAtPos( const QPoint & iPoint )
{
    QPoint mappedPoint = ApplyInvertTransformationComposition( iPoint );

    // Deepest child first
    for( auto child : mChildrenHUDs )
        if( child->Visible() && child->mOriginalFrame.contains( mappedPoint ) )
            return  child;

    // Then, if no child, myself
    if( Visible() && mOriginalFrame.contains( mappedPoint ) )
        return  this;

    // If i'm not in, nothing
    return  0;
}

