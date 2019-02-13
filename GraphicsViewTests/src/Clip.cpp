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
    DirtyArea( QRect( 0, 0, mWidth, mHeight ) );
}


void
cClip::DirtyArea( const QRect & iArea )
{
    mDirtyArea = mDirtyArea.united( iArea );

    if( mDirtyArea.right() <= 0 || mDirtyArea.bottom() <= 0
        || mDirtyArea.left() >= mWidth || mDirtyArea.top() >= mHeight )
    {
        mDirtyArea = QRect( 0, 0, 0, 0 );
        return;
    }

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


void
cClip::DirtyAll()
{
    mDirtyArea = QRect( 0, 0, mWidth, mHeight );
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
    {
        mCurrentLayer = mLayers.back();
        mSelection->SetOriginalImage( mCurrentLayer->Image() );
    }
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
            const int index = y * bpr + minX * 4;
            pixelRow        = renderData + index;
            originPixelRow  = originData + index;

            for( unsigned int x = minX; x < maxX; ++x )
            {
                const uint8_t alpha = originPixelRow[ 3 ];
                uchar sourceAlpha = alpha;
                if( sourceAlpha == 0 )
                {
                    originPixelRow += 4;
                    pixelRow += 4;
                    continue;
                }

                BlendPixelNormal( &pixelRow, *(originPixelRow+2), *(originPixelRow+1), *(originPixelRow), alpha );
                originPixelRow += 4;
            }
        }
        // /BLENDIMAGE

        if( mSelection->IsActive() && layer == mCurrentLayer )
        {
            QRect clippedArea = mDirtyArea.intersected( mSelection->GetTransformationBBox() );
            BlendImageNormalSameSizes( mSelection->TransformedImage(), mCurrentFrameRendering, clippedArea );
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
    mSelection->ExtractPixelsFromImageToBuffer();
}


