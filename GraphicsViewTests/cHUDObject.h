#pragma once


#include <QTransform>
#include <QPainter>

#include <QGenericMatrix>

class cHUDObject
{
public:
    ~cHUDObject();
    cHUDObject();

public:
    virtual  void   Draw( QPainter* iPainter ) = 0;

    virtual  void   SetFrame( const QRect& iFrame );
    virtual  void   MoveBy( const QPoint& iOffset );
    virtual  void   ScaleBy( float iScale );


    QRect   MappedRect( const QRect& iRect );
    QPoint  MappedPoint( const QPoint& iPoint );

protected:
    QMatrix3x2  mTransformation;

    QRect       mFrame;
    QRect       mOriginalFrame;
};

