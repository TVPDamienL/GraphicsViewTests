#include "Clip.h"

#include "cLayer.h"

#include "BenchmarkStuff.h"
#include "Math.Fast.h"

cClip::~cClip()
{
}


cClip::cClip( unsigned int iWidth, unsigned int iHeight ) :
    mName( "Layer" ),
    mWidth( iWidth ),
    mHeight( iHeight )
{
    mSelection = new cSelection( mWidth, mHeight );
    mCurrentFrameRendering = new QImage( mWidth, mHeight, QImage::Format_ARGB32_Premultiplied );
    mCurrentFrameRendering->fill( Qt::transparent );
    mDirtyArea = QRect( 0, 0, mWidth, mHeight );
}


void
cClip::DirtyArea( const QRect & iArea )
{
    mDirtyArea = mDirtyArea.united( iArea );
}


cLayer*
cClip::LayerAtIndex( int iIndex )
{
    return  mLayers[ iIndex ];;
}


void
cClip::AddLayer()
{
    mLayers.push_back( new cLayer( mWidth, mHeight ) );
}


QImage*
cClip::ComposeLayers()
{
    uchar* renderData = mCurrentFrameRendering->bits();
    uchar* pixelRow = renderData;

    int minX = mDirtyArea.left();
    int maxX = minX + mDirtyArea.width();
    int minY = mDirtyArea.top();
    int maxY = minY + mDirtyArea.height();


    for( auto layer : mLayers )
    {
        unsigned int bpr = layer->Image()->bytesPerLine();

        // BLENDIMAGE
        unsigned int index = 0;
        uchar* originData = layer->Image()->bits();
        uchar* originPixelRow = originData;

        for( unsigned int y = minY; y < maxY ; ++y )
        {
            pixelRow        = renderData + y * bpr + minX * 4;
            originPixelRow  = originData + y * bpr + minX * 4;

            for( unsigned int x = minX; x < maxX; ++x )
            {
                uchar sourceAlpha = originPixelRow[ 3 ];
                if( sourceAlpha == 0 )
                    continue;

                int transparencyAmountInverse = 255 - sourceAlpha;

                *pixelRow  = *originPixelRow + BlinnMult( *pixelRow, transparencyAmountInverse ); ++pixelRow; ++originPixelRow;
                *pixelRow  = *originPixelRow + BlinnMult( *pixelRow, transparencyAmountInverse ); ++pixelRow; ++originPixelRow;
                *pixelRow  = *originPixelRow + BlinnMult( *pixelRow, transparencyAmountInverse ); ++pixelRow; ++originPixelRow;
                *pixelRow  = *originPixelRow + BlinnMult( *pixelRow, transparencyAmountInverse ); ++pixelRow; ++originPixelRow;
            }
        }
        // /BLENDIMAGE
    }

    mDirtyArea = QRect( 0, 0, 0, 0 );

    return  mCurrentFrameRendering;
}


QImage*
cClip::GetOutputImage()
{
    QImage* output = ComposeLayers();

    return  output;
}


unsigned int
cClip::Width() const
{
    return  mWidth;
}


unsigned int
cClip::Height() const
{
    return  mHeight;
}


cSelection*
cClip::GetSelection()
{
    return  mSelection;
}


