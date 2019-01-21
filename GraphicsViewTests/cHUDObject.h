#pragma once


#include <QTransform>
#include <QPainter>

class cHUDObject
{
public:
    ~cHUDObject();
    cHUDObject();

public:
    virtual  void Draw( QPainter* iPainter ) = 0;

private:
    QTransform mTransformation;
};

