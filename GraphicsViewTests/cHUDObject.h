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

    virtual  void   SetFrame( const QRectF& iFrame );
    virtual  QRectF GetFrame() const;

    void            ResetTransformation();
    virtual  void   MoveBy( const QPointF& iOffset );
    virtual  void   ScaleBy( double iXScale, double iYScale );
    virtual  void   CenterScale( const QPointF& iCenter, double iXScale, double iYScale );
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
    QRectF   ToHUDCoords( const QRectF& iRect );
    QPointF  ToHUDCoords( const QPointF& iRect );
    QPointF  FromHUDCoords( const QPointF& iPoint ) const;

    QTransform GetFinalTransform() const;
    QTransform GetLocalTransform() const;


    virtual bool ContainsPoint( const QPointF& iPoint ) const;

    virtual  cHUDObject* GetVisibleHUDObjectAtPos( const QPointF& iPoint );

protected:
    cHUDView*               mParentView;
    cHUDObject*             mParentObject;
    QVector< cHUDObject* >  mChildrenHUDs;

    QRectF                  mOriginalFrame;
    QTransform              mObjectSelfTransformation;
    bool                    mVisible = true;

    QPointF                 mTranslation = QPointF( 0, 0 );
    double                  mXScale = 1.0;
    double                  mYScale = 1.0;
    double                  mRotationAngle = 0.0;
};

