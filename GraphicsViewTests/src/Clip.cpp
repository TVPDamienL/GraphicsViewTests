#include "Clip.h"

#include "cLayer.h"

#include "BenchmarkStuff.h"

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


    //Bench::ClockBegin();

    //for( int i = 0; i < 100; ++i )
    //{
        for( auto layer : mLayers )
        {
            unsigned int bpr = layer->Image()->bytesPerLine();

            // BLENDIMAGE
            //unsigned int index = 0;
            //const uchar* originData = layer->Image()->bits();
            //const uchar* originPixelRow = originData;

            //for( unsigned int y = minY; y < maxY ; ++y )
            //{
            //    pixelRow = renderData + y * bpr;
            //    originPixelRow = originData + y * bpr;

            //    for( unsigned int x = minX; x < maxX; ++x )
            //    {
            //        index = x * 4;
            //        uchar sourceAlpha = originPixelRow[ index + 3 ];
            //        if( sourceAlpha == 0 )
            //            continue;

            //        float transparencyAmountInverse = 1.F - ( float( sourceAlpha ) / 255.F );

            //        pixelRow[ index ]      = originPixelRow[ index + 0 ] + pixelRow[ index ]      * transparencyAmountInverse;
            //        pixelRow[ index + 1 ]  = originPixelRow[ index + 1 ] + pixelRow[ index + 1 ]  * transparencyAmountInverse;
            //        pixelRow[ index + 2 ]  = originPixelRow[ index + 2 ] + pixelRow[ index + 2 ]  * transparencyAmountInverse;
            //        pixelRow[ index + 3 ]  = originPixelRow[ index + 3 ] + pixelRow[ index + 3 ]  * transparencyAmountInverse;
            //    }
            //}
            // /BLENDIMAGE


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
    //}

    //qDebug() << "TIME : " << Bench::ClockEnd();

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


