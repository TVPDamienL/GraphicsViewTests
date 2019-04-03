#include "EditableItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>


// DEBUG
#include "Debug.h"


cEditableItem::cEditableItem( QGraphicsItem *parent ) :
    QGraphicsPixmapItem ( parent ),
    mImage( 0 )
{
    setAcceptHoverEvents( true );
}


QRectF
cEditableItem::boundingRect() const
{
    if( mImage )
        return QRectF( 0, 0, mImage->width(), mImage->height() );

    return  QRectF( 0, 0, 0, 0 );
}


void
cEditableItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if( mImage )
    {
        //painter->setRenderHint( QPainter::SmoothPixmapTransform, (transformationMode() == Qt::SmoothTransformation) );
        painter->drawImage( QPoint( 0, 0 ), *mImage );
    }


    auto  rect = boundingRect();
    int off = 1;
    rect.setLeft( rect.left() + off );
    rect.setTop( rect.top() + off );
    rect.setWidth( rect.width() - off );
    rect.setHeight( rect.height() - off );
    painter->drawRect( rect );

    //painter->setBrush( Qt::transparent );
    //QPen pen( QColor(0,0,0,120) );
    //pen.setWidthF( 0.5 );
    //painter->setPen( pen );
    //painter->setRenderHint( QPainter::Antialiasing, false );
    //for( auto& pt : debugDots )
    //{
    //    painter->drawPoint( pt );
    //}



    //for( auto& rrr : debugBBox )
    //{
    //    painter->drawRect( rrr );
    //}

    //pen.setColor( QColor( 0,255,0,120));
    //painter->setPen( pen );
    //for( auto& rrr : debugBBoxI )
    //{
    //    painter->drawRect( rrr );
    //}
}


void
cEditableItem::setFile( const QString & iFile )
{
    mFileToEdit = iFile.toStdString();
}


void cEditableItem::mousePressEvent( QGraphicsSceneMouseEvent* iEvent )
{
    QGraphicsItem::mousePressEvent( iEvent );
}


void cEditableItem::mouseMoveEvent( QGraphicsSceneMouseEvent* iEvent )
{
    QGraphicsItem::mouseMoveEvent( iEvent );
}


void cEditableItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *iEvent )
{
    QGraphicsItem::mouseReleaseEvent( iEvent );
}


void
cEditableItem::contextMenuEvent( QGraphicsSceneContextMenuEvent * iEvent )
{
    //QMenu menu;
    //menu.addAction("Remove Frame");
    //menu.addAction("Select File");
    //QAction *a = menu.exec( iEvent->screenPos() );
    //if( !a )
    //    return;

    //if( a->text() == "Remove Frame" )
    //{
    //    mParentView->ItemAskToBeRemoved( this ); // DON'T DO ANYTHING AFTER THIS
    //    return;
    //}
    //else if( a->text() == "Select File" )
    //{
    //    _SelectNewFile();
    //}
}

