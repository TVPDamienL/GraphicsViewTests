#pragma once

#include "cHUDObject.h"

#include "cHUDHandle.h"
#include "HUDHandlePath.h"
#include "PaintToolBase.h"
#include "Clip.h"

class cHUDPath :
    public cHUDObject
{
    enum eState : int
    {
        kIdle,
        kDragPosition,
        kDragPressure,
    };


public:
    virtual ~cHUDPath();
    cHUDPath( cHUDView* iParentView, cHUDObject* iParentObject, const std::vector< sPointData >& iPath, cPaintToolBase* tool, cClip* clip );

public:
    virtual  void   Draw( QPainter* iPainter ) override;
    virtual  void   SetFrame( const QRectF& iFrame ) override;

    virtual  bool   ContainsPoint( const QPointF& iPoint ) const override;

    virtual  bool  Event( QEvent* iEvent ) override;

    void SetHandleColor( const QColor& color );

private:
    void                _LayoutChildren();
    cHUDHandlePath*     _GetHandleAtPoint( int* oIndex, const QPointF& iPoint );
    int                 _GetHandleIndex( const cHUDObject* iObject );

private:
    eState                      mState = kIdle;
    cHUDHandlePath*             mFocusedHandle = 0;

    std::vector< sPointData >   mPath;
    cPaintToolBase*             mTool;
    cClip*                      mClip;

    QPoint                      mOriginClickPos;
    QRectF                      mOriginHandleFrame;
    cHUDObject*                 mOriginHandle;
    int                         mHandleIndex = -1;
};

