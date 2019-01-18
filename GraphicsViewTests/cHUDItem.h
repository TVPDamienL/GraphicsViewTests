#pragma once

#include <QGraphicsPixmapItem>

class cHUDItem :
    public QGraphicsPixmapItem
{

public:
    ~cHUDItem();
    cHUDItem( QGraphicsItem *parent = Q_NULLPTR );

public:
    QRectF  boundingRect() const override;
    void    paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

public:
    void SetPixmap( QPixmap*  iPixmap );

private:
    QPixmap* mPixmap;
};

