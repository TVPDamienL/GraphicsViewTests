#pragma once

#include <QGraphicsPixmapItem>
#include <QTimer>

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
    void SetSelectionOutlineImage( QImage*  iSelectionImage );
    void SetSelectionInsideImage( QImage*  iInsideImage );

private:
    void _RenderSelection();

private slots:
    void selectionTimerTimeout();

private:
    QImage*     mSelectionOutline = 0;
    QImage*     mSelectionInside = 0;
    QTimer*     mSelectionDrawingTimer;
    int         mBGOffset;
};

