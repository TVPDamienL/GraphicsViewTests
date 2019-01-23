#pragma once

#include "cHUDObject.h"

#include "cHUDHandle.h"

class cHUDTransform :
    public cHUDObject
{
public:
    ~cHUDTransform();
    cHUDTransform();

public:
    virtual  void   Draw( QPainter* iPainter ) override;
    virtual  void   SetFrame( const QRect& iFrame ) override;
    virtual  void   MoveBy( const QPoint& iOffset ) override;
    virtual  void   ScaleBy( float iScale ) override;

    virtual  bool   ContainsPoint( const QPoint& iPoint ) const override;

    virtual  bool  Event( QEvent* iEvent ) override;


private:
    void _LayoutChildren();

private:
    int                     mHandleSize = 10;
};

