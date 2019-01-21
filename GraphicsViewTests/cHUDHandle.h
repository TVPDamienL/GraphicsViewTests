#pragma once

#include "cHUDObject.h"

class cHUDHandle :
    public cHUDObject
{
public:
    ~cHUDHandle();
    cHUDHandle();

public:
    virtual  void Draw( QPainter* iPainter ) override;

private:
    int mSize = 10;
};

