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

public:
    void   setSize( float size );

public:
    void   SetFrame( const QRectF& iFrame ) override;
    QRectF GetFrame() const override;

public:
    const cHUDHandle* GetHandlePtr() const;

    virtual  cHUDObject* GetVisibleHUDObjectAtPos( const QPointF& iPoint ) override;


public:
    QColor mColor;
    float  mSize;

    cHUDHandle* mPressureHandle;
};

