#include "cHUDItem.h"

#include <QPainter>
#include <QtCore>

#include "BenchmarkStuff.h"

cHUDItem::~cHUDItem()
{
}


cHUDItem::cHUDItem( QGraphicsItem* iParent ) :
    QGraphicsPixmapItem( iParent )
{
    mBGOffset = 0;
    mSelectionOutline = 0;
    mSelectionDrawingTimer = new QTimer();
    mSelectionDrawingTimer->setInterval( 100 );
    mSelectionDrawingTimer->connect( mSelectionDrawingTimer, &QTimer::timeout, [ this ]{ this->_RenderSelection(); } );
}


QRectF
cHUDItem::boundingRect() const
{
    if( mSelectionOutline )
        return QRectF( 0, 0, mSelectionOutline->width(), mSelectionOutline->height() );

    return  QRectF( 0, 0, 0, 0 );
}


void
cHUDItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if( mSelectionOutline )
    {
        painter->setRenderHint( QPainter::NonCosmeticDefaultPen, ( transformationMode() == Qt::FastTransformation ) );

        if( mSelectionInside )
            painter->drawImage( offset(), *mSelectionInside );

        painter->drawImage( offset(), *mSelectionOutline );
    }
}


void
cHUDItem::SetSelectionOutlineImage( QImage*  iSelectionImage )
{
    mSelectionOutline = iSelectionImage; // iPixmap could be 0

    if( mSelectionOutline )
    {
        _RenderSelection();
        mSelectionDrawingTimer->start();
    }
    else
    {
        mSelectionDrawingTimer->stop();
    }

    update();
}


void
cHUDItem::SetSelectionInsideImage( QImage * iInsideImage )
{
    mSelectionInside = iInsideImage;
}


void
cHUDItem::_RenderSelection()
{
    uchar* data = mSelectionOutline->bits();
    uchar* dataScanline = data;

    unsigned int bytesPerLine = mSelectionOutline->bytesPerLine();
    unsigned int width = mSelectionOutline->width();
    unsigned int height = mSelectionOutline->height();

    mBGOffset = (mBGOffset + 1) % 20;

    dataScanline = data;
    for( int y = 0; y < width ; ++y )
    {
        for( int x = 0; x < height; ++x )
        {
            int alpha = *(dataScanline+3);

            if( alpha == 0 )
            {
                dataScanline += 4;
            }
            else
            {
                int color = 255;
                int xCell = (x+ mBGOffset) / 10 ;
                int yCell = (y+ mBGOffset) / 10 ;

                if( xCell%2 == yCell%2 )
                    color = 0;

                *dataScanline  = color; ++dataScanline;
                *dataScanline  = color; ++dataScanline;
                *dataScanline  = color; ++dataScanline;
                *dataScanline  = 255; ++dataScanline;
            }
        }
    }

    update();
}


