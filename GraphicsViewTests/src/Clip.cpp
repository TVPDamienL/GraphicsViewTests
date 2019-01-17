#include "Clip.h"

#include "cLayer.h"


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

    int minX = mDirtyArea.left();
    int maxX = minX + mDirtyArea.width();
    int minY = mDirtyArea.top();
    int maxY = minY + mDirtyArea.height();

    for( auto layer : mLayers )
    {
        unsigned int bpr = layer->Image()->bytesPerLine();

        // BLENDIMAGE
        unsigned int index = 0;
        const uchar* originData = layer->Image()->bits();

        for( unsigned int y = minY; y < maxY ; ++y )
        {
            for( unsigned int x = minX; x < maxX; ++x )
            {
                index = y * bpr + x * 4;
                uchar sourceAlpha = originData[ index + 3 ];
                if( sourceAlpha == 0 )
                    continue;

                float transparencyAmountInverse = 1.F - ( float( sourceAlpha ) / 255.F );

                renderData[ index ]      = originData[ index + 0 ] + renderData[ index ]      * transparencyAmountInverse;
                renderData[ index + 1 ]  = originData[ index + 1 ] + renderData[ index + 1 ]  * transparencyAmountInverse;
                renderData[ index + 2 ]  = originData[ index + 2 ] + renderData[ index + 2 ]  * transparencyAmountInverse;
                renderData[ index + 3 ]  = originData[ index + 3 ] + renderData[ index + 3 ]  * transparencyAmountInverse;
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


