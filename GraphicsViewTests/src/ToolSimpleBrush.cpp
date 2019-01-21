#include "ToolSimpleBrush.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Math.Fast.h"
#include "Blending.h"

cToolSimpleBrush::~cToolSimpleBrush()
{
    delete  mTipRendered;
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    ToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 10.0F; // Because we stay in squared numerics, to avoid the sqrt, so this would be a 5 pixel step
    mApplyProfile = true;

    mTipRendered = 0;
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


    mProfile.SetValueAtTime( 0.0, 0.0 );
    mProfile.SetValueAtTime( 0.5, 1.0 );
    mProfile.SetValueAtTime( 1.0, 1.0 );
}


void
cToolSimpleBrush::ApplyProfile( bool iApply )
{
    mApplyProfile = iApply;
}


bool
cToolSimpleBrush::ApplyProfile() const
{
    return  mApplyProfile;
}


void
cToolSimpleBrush::StartDrawing( QImage* iImage )
{
    ToolBase::StartDrawing( iImage );

    delete  mTipRendered;
    mTipRendered = new QImage( mToolSize, mToolSize, QImage::Format::Format_RGBA8888_Premultiplied );
    mTipRendered->fill( Qt::transparent );
    _DrawDot( mTipRendered, mToolSize/2, mToolSize/2, 1.0, 0.0 );
    mDirtyArea = QRect( 0, 0, 0, 0 );
}


QRect
cToolSimpleBrush::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  GetDirtyAreaAndReset();
}


void
cToolSimpleBrush::DrawDot( int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = mDrawingContext->bits();
    uchar* dataScanline = data;

    uchar* dataTip = mTipRendered->bits();
    uchar* dataTipScanline = dataTip;

    const uchar* alphaData = 0;
    const uchar* alphaScanline = 0;

    unsigned int bytesPerLine = mDrawingContext->bytesPerLine();
    unsigned int bytesPerLineTip = mTipRendered->bytesPerLine();

    int size = mToolSize * iPressure;
    QImage* scaledTip = mTipRendered;

    if( size != mToolSize )
        scaledTip = new QImage( mTipRendered->scaled( size, size, Qt::AspectRatioMode::KeepAspectRatio ) );

    int radius = scaledTip->width() / 2;

    float maxInverse = 1/255.F;

    int minX = iX - radius;
    int maxX = iX + radius;
    int minY = iY - radius;
    int maxY = iY + radius;

    // Basic out of bounds elimination
    if( minX >= mDrawingContext->width() || minY >= mDrawingContext->height() )
        return;
    if( maxX < 0 || maxY < 0 )
        return;

    int startingX = minX < 0 ? 0 : minX;
    int endingX = maxX >= mDrawingContext->width() ? mDrawingContext->width() - 1 : maxX;
    int startingY = minY < 0 ? 0 : minY;
    int endingY = maxY >= mDrawingContext->height() ? mDrawingContext->height() - 1 : maxY;

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX, endingY - startingY ) );

    bool useAlphaMask = mAlphaMask && mAlphaMask->width() == mDrawingContext->width() && mAlphaMask->height() == mDrawingContext->height();
    if( useAlphaMask )
    {
        alphaData = mAlphaMask->bits();
        alphaScanline = alphaData;
    }

    for( int y = startingY; y < endingY ; ++y )
    {
        int offset = y * bytesPerLine + startingX * 4;
        dataScanline = data +  offset;
        dataTipScanline = dataTip + (y - minY) * bytesPerLineTip + (startingX - minX ) * 4;

        if( useAlphaMask )
            alphaScanline = alphaData +  offset + 3;

        for( int x = startingX; x < endingX; ++x )
        {
            int srcB = *dataTipScanline; ++dataTipScanline;
            int srcG = *dataTipScanline; ++dataTipScanline;
            int srcR = *dataTipScanline; ++dataTipScanline;
            int srcA = *dataTipScanline; ++dataTipScanline;


            if( useAlphaMask )
            {
                int alphaMaskTransparency = *alphaScanline; alphaScanline += 4;
                srcR = BlinnMult( srcR, alphaMaskTransparency );
                srcG = BlinnMult( srcG, alphaMaskTransparency );
                srcB = BlinnMult( srcB, alphaMaskTransparency );
                srcA = BlinnMult( srcA, alphaMaskTransparency );
            }

            if( srcA == 0 )
            {
                dataScanline += 4;
            }
            else
            {
                BlendPixelNormal( &dataScanline, srcR, srcG, srcB, srcA );
            }
        }
    }
}


void
cToolSimpleBrush::DrawLine( int x1, int y1, int x2, int y2 )
{
    DrawDot( x1, y1, 1, 0 );
    DrawDot( x2, y2, 1, 0 );

    // All inbetweens
}


void
cToolSimpleBrush::_DrawDot( QImage * iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = iImage->bits();
    uchar* pixelRow = data;
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();
    uint8_t originR = mColor.red();
    uint8_t originG = mColor.green();
    uint8_t originB = mColor.blue();
    uint8_t originA = mColor.alpha();

    uint8_t finalR = mColor.red();
    uint8_t finalG = mColor.green();
    uint8_t finalB = mColor.blue();
    uint8_t finalA = mColor.alpha();

    const unsigned int bytesPerLine = iImage->bytesPerLine();

    const int r = (mToolSize/2) * iPressure;
    const int r2 = r*r;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    const float radiusSq = mToolSize * mToolSize / 4;

    bboxMinX = bboxMinX < 0 ? 0 : bboxMinX;
    bboxMaxX = bboxMaxX >= width ? width - 1 : bboxMaxX;

    bboxMinY = bboxMinY < 0 ? 0 : bboxMinY;
    bboxMaxY = bboxMaxY >= height ? height - 1 : bboxMaxY;

    const unsigned int iterationCount = (bboxMaxX - bboxMinX) * (bboxMaxY - bboxMinY);


    for( int y = bboxMinY; y <= bboxMaxY ; ++y )
    {
        pixelRow = data + y * bytesPerLine + bboxMinX * 4;

        for( int x = bboxMinX; x <= bboxMaxX ; ++x )
        {
            const int dx = x - iX;
            const int dy = y - iY;
            if( dx * dx + dy * dy <= r2 )
            {
                int xpos = x*4;

                if( mApplyProfile )
                {
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / radiusSq ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    uint8_t mult = mProfile.GetValueAtTime( distanceParam ) * 255.F;
                    finalR = BlinnMult( originR, mult );
                    finalG = BlinnMult( originG, mult );
                    finalB = BlinnMult( originB, mult );
                    finalA = BlinnMult( originA, mult );
                }

                BlendPixelNormal( &pixelRow, finalR, finalG, finalB, finalA );
            }
            else
            {
                pixelRow += 4;
            }
        }
    }
}




