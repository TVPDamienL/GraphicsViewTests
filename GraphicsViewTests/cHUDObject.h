#pragma once


#include <QGenericMatrix>
#include <QMouseEvent>
#include <QPainter>

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

    virtual  bool  Event( QEvent* iEvent );

    QRect   MappedRect( const QRect& iRect );
    QPoint  MappedPoint( const QPoint& iPoint );

    virtual bool ContainsPoint( const QPoint& iPoint ) const;

protected:
    QMatrix3x2  mTransformation;

    QRect       mFrame;
    QRect       mOriginalFrame;

    QVector< cHUDObject* > mChildrenHUDs;
};

