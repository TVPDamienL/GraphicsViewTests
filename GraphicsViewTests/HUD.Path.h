#pragma once

#include "cHUDObject.h"

#include "cHUDHandle.h"
#include "PaintToolBase.h"

class cHUDPath :
    public cHUDObject
{
public:
    virtual ~cHUDPath();
    cHUDPath( cHUDView* iParentView, cHUDObject* iParentObject, const std::vector< sPointData >& iPath );

public:
    virtual  void   Draw( QPainter* iPainter ) override;
    virtual  void   SetFrame( const QRectF& iFrame ) override;

    virtual  bool   ContainsPoint( const QPointF& iPoint ) const override;

    virtual  bool  Event( QEvent* iEvent ) override;

    void SetHandleColor( const QColor& color );

private:
    void            _LayoutChildren();
    cHUDHandle*     _GetHandleAtPoint( int* oIndex, const QPointF& iPoint );

private:
    cHUDHandle*     mFocusedHandle = 0;

    std::vector< sPointData > mPath;
};

