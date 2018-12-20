#pragma once

#include <QGraphicsPixmapItem>

#include <filesystem>

class  cCustomGraphicsView;

class cEditableItem :
    public QGraphicsPixmapItem
{
public:
    cEditableItem( QGraphicsItem *parent = Q_NULLPTR );

public:
    QRectF  boundingRect() const override;
    void    paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

public:
    void setFile( const QString& iFile );

protected:
    void        mousePressEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseMoveEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseReleaseEvent( QGraphicsSceneMouseEvent* iEvent ) override;

    void        contextMenuEvent( QGraphicsSceneContextMenuEvent* iEvent ) override;

public:
    QPixmap*    mpixmap;

private:
    std::filesystem::path  mFileToEdit;

};

