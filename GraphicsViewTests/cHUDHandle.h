#pragma once

#include "cHUDObject.h"

#define HANDLESIZE 10

class cHUDHandle :
    public cHUDObject
{
public:
    ~cHUDHandle();
    cHUDHandle( cHUDView* iParentView, cHUDObject* iParentObject );

public:
    virtual  void Draw( QPainter* iPainter ) override;
    virtual  bool  Event( QEvent* iEvent ) override;
};

