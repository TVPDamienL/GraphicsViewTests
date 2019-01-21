#pragma once


#include <QTransform>
#include <QPainter>

class cHUDObject
{
public:
    ~cHUDObject();
    cHUDObject();

public:
    virtual  void   Draw( QPainter* iPainter ) = 0;

    virtual  void   SetFrame( const QRect& iFrame );
    void  MoveBy( const QPoint& iOffset );

protected:
    QTransform  mTransformation;
    QRect       mFrame;
};

