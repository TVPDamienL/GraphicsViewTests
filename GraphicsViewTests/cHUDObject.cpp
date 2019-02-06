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
    _mCosAngle = 1;
    _mSinAngle = 0;
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
cHUDObject::RotateBy( double iAngle )
{
    mRotationAngle += iAngle;
    _mCosAngle = cos( mRotationAngle );
    _mSinAngle = sin( mRotationAngle );
}


void
cHUDObject::CenterScale( const QPointF & iCenter, double iXScale, double iYScale )
{
    const double xPart = mXScale * (iCenter.x() - iXScale * iCenter.x());
    const double yPart = mYScale * (iCenter.y() - iYScale * iCenter.y());

    mTranslation.setX( _mCosAngle * xPart - _mSinAngle * yPart + mTranslation.x() );
    mTranslation.setY( _mSinAngle * xPart + _mCosAngle * yPart + mTranslation.y() );

    ScaleBy( iXScale, iYScale );
}


void
cHUDObject::CenterRotation( const QPointF & iCenter, double iAngle )
{
    const double cosAngle = cos( iAngle );
    const double sinAngle = sin( iAngle );

    //const double xPart = (sinAngle * iCenter.y() - cosAngle * iCenter.x() + iCenter.x()) * mXScale;
    //const double yPart =  (-cosAngle * iCenter.y() + iCenter.y() - sinAngle * iCenter.x()) * mYScale;

    //mTranslation.setX( _mCosAngle * xPart - _mSinAngle * yPart + mTranslation.x() );
    //mTranslation.setY( _mSinAngle * xPart + _mCosAngle * yPart + mTranslation.y() );

    // C.iS.T.R.iT.S TODO: explain why

    const double xPart = sinAngle * iCenter.y() - cosAngle * iCenter.x() + iCenter.x();
    const double yPart = -cosAngle * iCenter.y() + iCenter.y() - sinAngle * iCenter.x();

    mTranslation.setX( _mCosAngle * xPart - _mSinAngle * yPart / mYScale + mTranslation.x() );
    mTranslation.setY( _mSinAngle * xPart / mXScale + _mCosAngle * yPart + mTranslation.y() );


    RotateBy( iAngle );
}


void
cHUDObject::CenterRotationPostTransform( const QPointF & iCenter, double iAngle )
{
    const double cosAngle = cos( iAngle );
    const double sinAngle = sin( iAngle );

    const double xPart = mTranslation.x() - iCenter.x();
    const double yPart = mTranslation.y() - iCenter.y();

    mTranslation.setX( cosAngle * xPart - sinAngle * yPart + iCenter.x() );
    mTranslation.setY( sinAngle * xPart + cosAngle * yPart + iCenter.y() );


    RotateBy( iAngle );
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


QPolygonF
cHUDObject::ToHUDCoords( const QRectF & iRect )
{
    return  GetFinalTransform().mapToPolygon( QRect( iRect.x(), iRect.y(), iRect.width(), iRect.height() ) );
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
    QTransform rotate = QTransform().rotate( mRotationAngle * 180 / PI );
    return QTransform::fromScale( mXScale, mYScale ) * rotate * QTransform::fromTranslate( mTranslation.x(), mTranslation.y() );
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

