#pragma once

#include "cHUDObject.h"

#include "cHUDHandle.h"
#include "cSelection.h"

class cHUDTransform :
    public cHUDObject
{
public:
    ~cHUDTransform();
    cHUDTransform( cHUDView* iParentView, cHUDObject* iParentObject );

public:
    virtual  void   Draw( QPainter* iPainter ) override;
    virtual  void   SetFrame( const QRectF& iFrame ) override;

    virtual  bool   ContainsPoint( const QPointF& iPoint ) const override;

    virtual  bool  Event( QEvent* iEvent ) override;

public:
    void           SetSelection( cSelection* iSelection );
    void           SelectionChangedEvent( cBaseData* iSender, int iArg );

private:
    void            _LayoutChildren();
    cHUDHandle*     _GetHandleAtPoint( int* oIndex, const QPointF& iPoint );
    cHUDHandle*     _GetOppositeHandle( int iIndex );

    // Returns the transformation that transforms this object, having the top left corner as origin
    // So this represents the transformation this hud had from his original position
    // It removes scale so we can apply it aside
    // Because we wanna use QT's rescale method, and then apply rotation + translation ourselves
    // So we pass scale aside, and get a scale free matrix
    QTransform      _GetLocalTransformWithTopLeftOriginNoScale() const;

private:
    int             mHandleSize = 10;
    cHUDHandle*     mFocusedHandle = 0;
    cHUDHandle*     mFocusedHandleOpposite = 0;
    cSelection*     mSelection;

    QPoint          mClickOrigin;
    QPointF         mOriginHandleVector;
    QPointF         mOriginTranslation;
    double          mOriginXScale;
    double          mOriginYScale;
    double          mOriginGlobalXScale;
    double          mOriginGlobalYScale;
    double          mOriginRotation;
};

