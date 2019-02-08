#pragma once

#include "cHUDObject.h"

#include <QWidget>


class cHUDView :
    public QWidget
{
    Q_OBJECT
public:
    ~cHUDView();
    cHUDView( QWidget* iParent = nullptr );

public:
    void paintEvent( QPaintEvent* iEvent ) override;

public:
    void  tabletEvent( QTabletEvent*  iEvent ) override;

    void  dragEnterEvent( QDragEnterEvent* iEvent ) override;
    void  dragMoveEvent( QDragMoveEvent* iEvent ) override;
    void  dragLeaveEvent( QDragLeaveEvent* iEvent ) override;
    void  dropEvent( QDropEvent* iEvent ) override;

    void  keyPressEvent( QKeyEvent* iEvent ) override;
    void  keyReleaseEvent( QKeyEvent* iEvent ) override;

    void  mousePressEvent( QMouseEvent* iEvent )    override;
    void  mouseMoveEvent( QMouseEvent* iEvent )     override;
    void  mouseReleaseEvent( QMouseEvent* iEvent )  override;
    void  wheelEvent( QWheelEvent* iEvent )         override;

public:
    QTransform      GetTransform();
    void            TranslateBy( const QPointF& iOffset );
    void            ScaleBy( double iScale );

    const QPointF&  Translation() const;
    double          Scale() const;
    double          RotationAngle() const;

    void            SetTranslation( const QPointF& iTranslation );
    void            SetScale( double iScale );
    void            SetRotation( double iAngle );

public:
    void        AddHUDObject( cHUDObject* iObject );
    void        RemoveHUDObject( cHUDObject* iObject );
    cHUDObject* GetVisibleHUDObjectAtPos( const QPointF& iPoint );

private:
    void  _RecomputeMatrix();

private:
    QVector< cHUDObject* >  mHUDObjects;
    cHUDObject*             mPressedHUD = 0;

    // Transform
    QTransform  mTransform;
    QPointF     mTranslation = QPointF( 0, 0 );
    double      mRotationAngle = 0.0F;
    double      _mCosAngle = 1;
    double      _mSinAngle = 0;
    double      mScale = 1.0F;

};

