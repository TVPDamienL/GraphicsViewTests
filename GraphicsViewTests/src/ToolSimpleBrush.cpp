#include "ToolSimpleBrush.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    ToolBase( iParent )
{
    mToolSize = 5;
    mColor = Qt::red;
    mStep = 10.0F; // Because we stay in squared numerics, to avoid the sqrt, so this would be a 5 pixel step

    buildTool();
}


Qt::ItemFlags
cToolSimpleBrush::flags( const QModelIndex & iIndex ) const
{
    return  ToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolSimpleBrush::buildTool()
{
    QStandardItem* sizeItem = new QStandardItem( "Size" );
    sizeItem->setData( mToolSize );
    setItem( 0, 0, sizeItem );
    QStandardItem* color = new QStandardItem( "Color" );
    color->setData( mColor );
    setItem( 1, 0, color );
    QStandardItem* step = new QStandardItem( "Step" );
    step->setData( mStep );
    setItem( 2, 0, step );

}


void
cToolSimpleBrush::_CircleMethod( QImage* iImage, int iX, int iY, int iP, int iQ )
{
    uchar* data = iImage->bits();
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();
    int R = mColor.red();
    int G = mColor.green();
    int B = mColor.blue();
    int A = mColor.alpha();

    _DrawPixel( data, width, height, iX + iP, iY + iQ, R, G, B, A );
    _DrawPixel( data, width, height, iX - iP, iY + iQ, R, G, B, A );
    _DrawPixel( data, width, height, iX + iP, iY - iQ, R, G, B, A );
    _DrawPixel( data, width, height, iX - iP, iY - iQ, R, G, B, A );
    _DrawPixel( data, width, height, iX + iQ, iY + iP, R, G, B, A );
    _DrawPixel( data, width, height, iX - iQ, iY + iP, R, G, B, A );
    _DrawPixel( data, width, height, iX + iQ, iY - iP, R, G, B, A );
    _DrawPixel( data, width, height, iX - iQ, iY - iP, R, G, B, A );
}


void
cToolSimpleBrush::_DrawPixel( uchar * iData, unsigned int iImageWidth, unsigned int iImageHeight, int iX, int iY, int iR, int iG, int iB, int iA )
{
    int x = iX < 0 ? 0 : iX;
    x = x > iImageWidth ? iImageWidth-1 : x;

    int y = iY < 0 ? 0 : iY;
    y = y > iImageHeight ? iImageHeight-1 : y;


    int index = y * iImageWidth * 4 + x * 4;

    // BGRA format
    iData[ index ] = iB;
    iData[ index + 1 ] = iG;
    iData[ index + 2 ] = iR;
    iData[ index + 3 ] = iA;
}


void
cToolSimpleBrush::DrawDot( QImage* iImage, int x, int y )
{
    int centerX = x;
    int centerY = y;
    int radius = mToolSize/2;
    int P = 0;
    int Q = radius;
    int D = 3 - 2*radius;
    while( P <= Q )
    {
        _CircleMethod( iImage, centerX, centerY, P, Q );
        ++P
            ;
        if( D < 0 )
        {
            D = D + 4*P + 6;
        }
        else
        {
            --radius;
            D = D + 4 * (P-Q) + 10;
        }

        _CircleMethod( iImage, centerX, centerY, P, Q );
    }
}


void
cToolSimpleBrush::DrawLine( QImage * iImage, int x1, int y1, int x2, int y2 )
{
    DrawDot( iImage, x1, y1 );
    DrawDot( iImage, x2, y2 );

    // All inbetweens
}




