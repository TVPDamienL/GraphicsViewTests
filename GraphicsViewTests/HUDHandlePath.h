#pragma once

#include "cHUDObject.h"

class cHUDHandle;

class cHUDHandlePath :
    public cHUDObject
{
public:
    ~cHUDHandlePath();
    cHUDHandlePath( cHUDView* iParentView, cHUDObject* iParentObject );

public:
    virtual  void  Draw( QPainter* iPainter ) override;
    virtual  bool  Event( QEvent* iEvent ) override;

    virtual  bool   ContainsPoint( const QPointF& iPoint ) const override;

public:
    void   SetFrame( const QRectF& iFrame ) override;
    QRectF GetFrame() const override;

public:
    const cHUDHandle* GetHandlePtr() const;


public:
    QColor mColor;

    cHUDHandle* mPressureHandle;
};

