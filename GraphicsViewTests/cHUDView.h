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
    QTransform& GetTransform();
    void  TranslateBy( const QPoint& iOffset );
    void  ScaleBy( double iScale );
    void  SetDrawingAreaOffset( const QPoint& iOffset );
    double Scale() const;


public:
    void        AddHUDObject( cHUDObject* iObject );
    cHUDObject* GetVisibleHUDObjectAtPos( const QPoint& iPoint );


public:
    QPoint  MapToView( const QPoint& iPoint );
    QRect   MapToView( const QRect& iRect );


private:
    QVector< cHUDObject* >  mHUDObjects;
    cHUDObject*             mPressedHUD = 0;

    QTransform              mGlobalTransformation;

    QPoint  mDrawingAreaOffset;
    double  mScale = 1.0F;
};

