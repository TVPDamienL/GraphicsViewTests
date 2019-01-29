#pragma once

#include <QGraphicsView>
#include <QPaintEvent>

#include "ToolBase.h"
#include "Clip.h"
#include "cGridItem.h"
#include "cHUDItem.h"
#include "cHUDView.h"
#include "cHUDTransform.h"

class cEditableItem;

class cCanvas :
    public QGraphicsView
{
    Q_OBJECT

public:
    cCanvas( QWidget *parent = nullptr );

public:
    void  resizeEvent( QResizeEvent* iEvent ) override;

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
    void  SetPixmap( const QPixmap& iPixmap );
    void  SetClip( cClip* iClip );
    void  SetToolModel( ToolBase* iToolModel );
    void  SetSelectionMode( bool iMode );

    void  UpdateGridItem();
    void  UpCursor();
    void  DrawCursor();
    void  toolChanged( const QModelIndex& Left, const QModelIndex& Right, const QVector< int >& Roles );

public:
    void  ScaleFromCenter( const QPoint& iCenter, double iScale );

signals:
    void  currentFrameGotPainted( const QPixmap& iPixmap );
    void  previousFrameGotPainted( const QPixmap& iPixmap );
    void  zoomChanged( double iValue );

public slots:
    void  SetZoom( double iZoom );

private:
    void  _SetImage( const QImage* iImage );


private:
    enum  eState
    {
        kIdle,
        kPan,
        kZoom,
        kDrawing
    };

    enum  eTool
    {
        kBrush,
        kEraser
    };

    eState              mState;

    cEditableItem*      mEditableItem;
    cGridItem*          mGridItem;

    // HUDs
    cHUDView*           mHUDView;
    cHUDItem*           mHUDSelection;
    cHUDTransform*      mHUDTransform;

    // Transform
    QPointF             mTranslation = QPointF( 0, 0 );
    double              mRotationAngle = 0.0F;
    double              mScale = 1.0F;
    QTransform          mTransform;


    QPointF             mClickPos;
    QPixmap*            mItemPixmap;

    bool                mSelectionMode = false;

    eTool               mTool;
    ToolBase*           mToolModel;
    QPainter*           mPainter;
    cClip*              mClip;

    QPixmap*            cursorPixmap;

    QImage*             __DebugAlphaMaskTest__;
};

