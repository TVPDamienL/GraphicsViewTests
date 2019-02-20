#include "Clip.h"

#include "cLayer.h"

#include "BenchmarkStuff.h"
#include "Math.Fast.h"
#include "Blending.h"

#include "Image.Utilities.h"
#include "GPUForThisApp.h"

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
    _GPU->SetLayersByteSize( mCurrentFrameRendering->sizeInBytes() );
    _GPU->SetLayersBytePerLine( mCurrentFrameRendering->bytesPerLine() ); // Very abusive, as we assume all image of same size have same BPL
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

    // TMP hack to see stuff
    mCurrentLayer = mLayers.back();

    _GPU->LoadLayerToGPU( mLayers.back()->Image() );
}


QImage*
cClip::ComposeLayers()
{
    int minX = mDirtyArea.left();
    int maxX = minX + mDirtyArea.width();
    int minY = mDirtyArea.top();
    int maxY = minY + mDirtyArea.height();

#define GPU
#ifdef GPU
    _GPU->FillImageGPU( mCurrentFrameRendering, mDirtyArea, Qt::transparent, 1 );
    _GPU->PerformLayerCompositing( mCurrentFrameRendering, mDirtyArea );

    for( auto layer : mLayers )
    {
        //_GPU->BlendImageSameSizesGPU(  layer->Image(), mCurrentFrameRendering, mDirtyArea, 0 );

        // REMOVE this when benchmarking
        if( mSelection->IsActive() && layer == mCurrentLayer )
        {
            QRect clippedArea = mDirtyArea.intersected( mSelection->GetTransformationBBox() );
            BlendImageNormalSameSizes( mSelection->TransformedImage(), mCurrentFrameRendering, clippedArea );

            //QRect clippedArea = mDirtyArea.intersected( mSelection->GetTransformationBBox() );
            //_GPU->BlendImageSameSizesGPU(  mSelection->TransformedImage(), mCurrentFrameRendering, clippedArea, 0 );

        }
        // /REMOVE this when benchmarking
    }
#else
    HardFill( mCurrentFrameRendering, mDirtyArea, Qt::transparent );

    for( auto layer : mLayers )
    {
        BlendImageNormalSameSizes( layer->Image(), mCurrentFrameRendering, mDirtyArea );

        // REMOVE this when benchmarking
        if( mSelection->IsActive() && layer == mCurrentLayer )
        {
            QRect clippedArea = mDirtyArea.intersected( mSelection->GetTransformationBBox() );
            BlendImageNormalSameSizes( mSelection->TransformedImage(), mCurrentFrameRendering, clippedArea );

        }
        // /REMOVE this when benchmarking
    }
#endif

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


