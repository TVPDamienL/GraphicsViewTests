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
cClip::ComposeLayers() const
{
    QImage* output = new QImage( mWidth, mHeight, QImage::Format::Format_RGBA8888_Premultiplied );
    output->fill( Qt::transparent );
    uchar* outputData = output->bits();

    for( auto layer : mLayers )
    {
        // BLENDIMAGE
        unsigned int index = 0;
        const uchar* originData = layer->Image()->bits();

        for( unsigned int y = 0; y < mHeight - 1 ; ++y )
        {
            for( unsigned int x = 0; x < mWidth; ++x )
            {
                index = y * layer->Image()->bytesPerLine() + x * 4;
                uchar sourceAlpha = originData[ index + 3 ];
                if( sourceAlpha == 0 )
                    continue;

                float transparencyAmountInverse = 1.F - ( float( sourceAlpha ) / 255.F );

                outputData[ index ]      = originData[ index + 0 ] + outputData[ index ]      * transparencyAmountInverse;
                outputData[ index + 1 ]  = originData[ index + 1 ] + outputData[ index + 1 ]  * transparencyAmountInverse;
                outputData[ index + 2 ]  = originData[ index + 2 ] + outputData[ index + 2 ]  * transparencyAmountInverse;
                outputData[ index + 3 ]  = originData[ index + 3 ] + outputData[ index + 3 ]  * transparencyAmountInverse;
            }
        }
        // /BLENDIMAGE
    }

    return  output;
}


QImage*
cClip::GetOutputImage() const
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


