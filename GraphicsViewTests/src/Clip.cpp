#include "Clip.h"

#include "cLayer.h"

#include "BenchmarkStuff.h"
#include "Math.Fast.h"
#include "Blending.h"
#include "Image.Utilities.h"

cClip::~cClip()
{
}


cClip::cClip( unsigned int iWidth, unsigned int iHeight ) :
    mName( "Layer" ),
    mWidth( iWidth ),
    mHeight( iHeight )
{
    mSelection = new cSelection( mWidth, mHeight, this );
    mCurrentFrameRendering = new QImage( mWidth, mHeight, QImage::Format_ARGB32_Premultiplied );
    mCurrentFrameRendering->fill( Qt::transparent );
    mDirtyArea = QRect( 0, 0, mWidth, mHeight );
    mDirtyArea = QRect( 0, 0, 0, 0 );
}


void
cClip::DirtyArea( const QRect & iArea )
{
    mDirtyArea = mDirtyArea.united( iArea );

    // Clipping to clip
    if( mDirtyArea.left() < 0 )
        mDirtyArea.setLeft( 0 );

    if( mDirtyArea.top() < 0 )
        mDirtyArea.setTop( 0 );

    if( mDirtyArea.right() >= mWidth )
        mDirtyArea.setRight( mWidth - 1 );

    if( mDirtyArea.bottom() >= mHeight )
        mDirtyArea.setBottom( mHeight - 1 );

    EmitValueChanged( kDirty );
}


cLayer*
cClip::LayerAtIndex( int iIndex )
{
    return  mLayers[ iIndex ];;
}


cLayer*
cClip::CurrentLayer()
{
    return  mCurrentLayer;
}


void
cClip::AddLayer()
{
    mLayers.push_back( new cLayer( mWidth, mHeight ) );
    if( !mCurrentLayer )
        mCurrentLayer = mLayers.back();
}


QImage*
cClip::ComposeLayers()
{
    int minX = mDirtyArea.left();
    int maxX = minX + mDirtyArea.width();
    int minY = mDirtyArea.top();
    int maxY = minY + mDirtyArea.height();

    HardFill( mCurrentFrameRendering, mDirtyArea, Qt::transparent );

    uchar* renderData = mCurrentFrameRendering->bits();
    uchar* pixelRow = renderData;

    for( auto layer : mLayers )
    {
        unsigned int bpr = layer->Image()->bytesPerLine();

        // BLENDIMAGE - optimized with dirty area
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
                {
                    originPixelRow += 4;
                    pixelRow += 4;
                    continue;
                }

                BlendPixelNormal( &pixelRow, *(originPixelRow+2), *(originPixelRow+1), *(originPixelRow), *(originPixelRow+3) ); originPixelRow += 4;
            }
        }
        // /BLENDIMAGE

        if( mSelection->IsActive() && layer == mCurrentLayer )
        {
            BlendImageNormalSameSizes( mSelection->TransformedImage(), mCurrentFrameRendering, mSelection->GetTransformationBBox() );
        }
    }

    EmitValueChanged( kRenderedDirtyArea );

    mDirtyArea = QRect( 0, 0, 0, 0 );

    return  mCurrentFrameRendering;
}


QImage*
cClip::GetOutputImage()
{
    QImage* output = ComposeLayers();

    return  output;
}


int
cClip::Width() const
{
    return  mWidth;
}


int
cClip::Height() const
{
    return  mHeight;
}


cSelection*
cClip::GetSelection()
{
    return  mSelection;
}


void
cClip::ExtractSelection()
{
    mSelection->ExtractPixelsFromImageToBuffer( mCurrentFrameRendering );
}


