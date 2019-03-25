#pragma once


#include <QGenericMatrix>
#include <QMouseEvent>
#include <QPainter>


#define PI 3.14159265359

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

    virtual  void   SetFrame( const QRectF& iFrame );
    virtual  QRectF GetFrame() const;

    void            ResetTransformation();
    virtual  void   MoveBy( const QPointF& iOffset );
    virtual  void   ScaleBy( double iXScale, double iYScale );
    void            RotateBy( double iAngle );
    virtual  void   CenterScale( const QPointF& iCenter, double iXScale, double iYScale );

    // Pre transform = center must be in original coordinates
    // Doesn't work though
    virtual  void   CenterRotation( const QPointF& iCenter, double iAngle );

    // Post transform = center must be in local transform coordinates
    virtual  void   CenterRotationPostTransform( const QPointF& iCenter, double iAngle );
    double          LocalXScale() const;
    double          LocalYScale() const;
    double          GlobalXScale() const;
    double          GlobalYScale() const;
    QPointF         GlobalTranslation() const;

    virtual  bool   Event( QEvent* iEvent );

public:
    bool            Visible() const;
    void            Visible( bool iVisible );

public:
    QPolygonF   ToHUDCoords( const QRectF& iRect );
    QPointF     ToHUDCoords( const QPointF& iRect );
    QPointF     FromHUDCoords( const QPointF& iPoint ) const;

    QTransform GetFinalTransform() const;
    QTransform GetLocalTransform() const;


    virtual bool ContainsPoint( const QPointF& iPoint ) const;
    cHUDObject* GetObjectAtPoint( const QPointF& iPoint );

    virtual  cHUDObject* GetVisibleHUDObjectAtPos( const QPointF& iPoint );

protected:
    cHUDView*               mParentView;
    cHUDObject*             mParentObject;
    QVector< cHUDObject* >  mChildrenHUDs;

    QRectF                  mOriginalFrame;
    bool                    mVisible = true;

    QPointF                 mTranslation = QPointF( 0, 0 );
    double                  mXScale = 1.0;
    double                  mYScale = 1.0;
    double                  mRotationAngle = 0.0;
    double                  _mCosAngle = 1;
    double                  _mSinAngle = 0;
};

