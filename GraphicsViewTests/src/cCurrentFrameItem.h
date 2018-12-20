#pragma once

#include <QGraphicsItem>

class  cCustomGraphicsView;

class cCurrentFrameItem :
    public QGraphicsItem
{
public:
    cCurrentFrameItem( cCustomGraphicsView* iParentView, QGraphicsItem *parent = Q_NULLPTR );

public:
    void setIndex( int iIndex );

public:
    void  setPlaying( bool iPlaying );

public:
    QRectF  boundingRect() const override;
    void    paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

protected:
    void        mousePressEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseMoveEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseReleaseEvent( QGraphicsSceneMouseEvent* iEvent ) override;

    QVariant    itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    int                     mIndex;
    QColor                  mColor;
    cCustomGraphicsView*    mParentView;

    bool                    mPlaying;
};

