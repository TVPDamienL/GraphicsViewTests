#pragma once

#include <QGraphicsPixmapItem>

class  cCustomGraphicsView;

class cGraphicItem :
    public QGraphicsPixmapItem
{
public:
    cGraphicItem( cCustomGraphicsView* iParentView, QGraphicsItem *parent = Q_NULLPTR );

public:
    void setIndex( int iIndex );
    void setFile( const QString& iFile );

public:
    void  setHighlighted( bool iHighlighted );

public:
    void    paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

protected:
    void        mousePressEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseMoveEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseReleaseEvent( QGraphicsSceneMouseEvent* iEvent ) override;
    void        mouseDoubleClickEvent( QGraphicsSceneMouseEvent* iEvent ) override;

    void        contextMenuEvent( QGraphicsSceneContextMenuEvent* iEvent ) override;

    QVariant    itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void  _SelectNewFile();

private:
    QString                 mText;
    int                     mIndex;
    QColor                  mColor;
    cCustomGraphicsView*    mParentView;

    bool                    mPlaybackHighlight;
};

