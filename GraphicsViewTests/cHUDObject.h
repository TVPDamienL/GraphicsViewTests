#pragma once


#include <QGenericMatrix>
#include <QMouseEvent>
#include <QPainter>

class cHUDView;

class cHUDObject
{
public:
    ~cHUDObject();
    cHUDObject( cHUDView* iParentView, cHUDObject* iParentObject );


public:
    cHUDObject*     ParentObject();

public:
    virtual  void   Draw( QPainter* iPainter ) = 0;

    virtual  void   SetFrame( const QRect& iFrame );
    virtual  QRect  GetFrame() const;

    virtual  void   MoveBy( const QPoint& iOffset );
    virtual  void   ScaleBy( float iScale );
    float           Scale() const;

    virtual  bool  Event( QEvent* iEvent );

    QTransform*    GetTransform();


public:
    QRect   MapToObject( const QRect& iRect );
    QPoint  MapToObject( const QPoint& iPoint );

    QPoint  ApplyInvertTransformationComposition( const QPoint& iPoint ) const;
    QPoint  ApplyTransformationComposition( const QPoint& iPoint ) const;

    virtual bool ContainsPoint( const QPoint& iPoint ) const;

    virtual  cHUDObject* GetHUDObjectAtPos( const QPoint& iPoint );

protected:
    cHUDView*               mParentView;
    cHUDObject*             mParentObject;
    QVector< cHUDObject* >  mChildrenHUDs;

    QRect       mOriginalFrame;
    QTransform  mObjectSelfTransformation;

    float mScale = 1.0F;

};

