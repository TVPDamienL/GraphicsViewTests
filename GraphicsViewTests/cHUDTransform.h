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
    virtual  void Draw( QPainter* iPainter ) override;
    virtual  void SetFrame( const QRect& iFrame ) override;

private:
    void _LayoutChildren();

private:
    QVector< cHUDHandle* >  mHandles;
    int                     mHandleSize = 10;
};

