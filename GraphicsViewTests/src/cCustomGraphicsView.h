#pragma once

#include <QGraphicsView>

class cGraphicItem;
class cCurrentFrameItem;
class cAddItem;
class cClip;

class cCustomGraphicsView :
    public QGraphicsView
{
    Q_OBJECT

public:
    cCustomGraphicsView( QWidget *parent = nullptr );

public:
    void  dragEnterEvent( QDragEnterEvent* iEvent ) override;
    void  dragMoveEvent( QDragMoveEvent* iEvent ) override;
    void  dragLeaveEvent( QDragLeaveEvent* iEvent ) override;
    void  dropEvent( QDropEvent* iEvent ) override;

    void  keyPressEvent( QKeyEvent* iEvent ) override;

    void  mousePressEvent( QMouseEvent* iEvent ) override;
    void  mouseReleaseEvent( QMouseEvent* iEvent ) override;

public:
    void  AddItem();
    void  ItemMoving( cGraphicItem* iItem, const QPointF& iNewPosition );
    void  ItemMoved();
    void  ItemCurrentFrameMoved();

    void  ItemAskToBeRemoved( cGraphicItem* iItem );

    QVector< cGraphicItem* >& GetAnimationImages();

public:
    void  CurrentFrameChanged( int iCurrent );
    void  CurrentFrameGotPainted( const QPixmap& iNewPixmap );
    void  PreviousFrameGotPainted( const QPixmap& iNewPixmap );

signals:
        void  currentFrameChanged( int iNewIndex );

private:
    void  _UpdateItemsPosition();
    void  _UpdateCurrentFrameItemPosition();
    void  _SortItems();
    void  _UpdateSceneRect();

private:
    cAddItem*                   mAddItem;
    cCurrentFrameItem*          mCurrentFrameItem;
    QVector< cGraphicItem* >    mAnimationImages;
    int                         mCurrentFrame;
    QPoint                      mOriginPos;
};

