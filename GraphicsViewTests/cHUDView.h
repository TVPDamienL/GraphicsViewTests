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
    void  ApplyPan( const QPoint& iOffset );
    void  ApplyZoom( float iScale );
    void  SetDrawingAreaOffset( const QPoint& iOffset );

    float Scale() const;


private:
    QVector< cHUDObject* > mHUDObjects;
    QPoint  mDrawingAreaOffset;
    float   mScale = 1.0F;
};

