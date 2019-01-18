#include "cHUDItem.h"

#include <QPainter>

cHUDItem::~cHUDItem()
{
}


cHUDItem::cHUDItem( QGraphicsItem* iParent ) :
    QGraphicsPixmapItem( iParent )
{
    mPixmap = 0;
}


QRectF
cHUDItem::boundingRect() const
{
    if( mPixmap )
        return QRectF( 0, 0, mPixmap->width(), mPixmap->height() );

    return  QRectF( 0, 0, 0, 0 );
}


void
cHUDItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if( mPixmap )
    {
        painter->setRenderHint( QPainter::SmoothPixmapTransform, ( transformationMode() == Qt::SmoothTransformation ) );
        painter->drawPixmap( offset(), *mPixmap );
    }
}


void
cHUDItem::SetPixmap( QPixmap*  iPixmap )
{
    delete  mPixmap;
    mPixmap = iPixmap;
    update();
}


