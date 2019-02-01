#include "cSelection.h"

#include "Image.Utilities.h"
#include "Clip.h"

cSelection::~cSelection()
{
}


cSelection::cSelection( int iWidth, int iHeight, cClip* iClip )
{
    mAssociatedClip         = iClip;
    mMaskImage              = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    mTransformationBuffer   = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    mEdgeDetectedMaskImage  = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    mOriginalImage          = 0;
    Clear();
}


QImage*
cSelection::GetSelectionMask()
{
    return  mMaskImage;
}


QImage*
cSelection::GetSelectionContentImage()
{
    return  mTransformationBuffer;
}


bool
cSelection::IsActive() const
{
    return  mActive;
}


void
cSelection::SetActive( bool iActive )
{
    mActive = iActive;
    EmitValueChanged( kActiveChanged );
}


void
cSelection::EmitPainted()
{
    EmitValueChanged( kPainted );
}


void
cSelection::Clear()
{
    mMaskImage->fill( Qt::transparent );
    SetActive( false );
}


void
cSelection::ProcessEdgeDetection()
{
    _FilterAlpha();
    mEdgeDetectedMaskImage = mEdgeDetectionConvolution.GetEdgeDectionFromImage( mEdgeDetectedMaskImage );
}


void
cSelection::ExtractPixelsFromImageToBuffer( QImage * iImage )
{
    mOriginalImage = iImage;
    const QRect selectionBBox = GetSelectionBBox();
    const int bboxWidth = mSelectionBBox.width();
    const int bboxHeight = mSelectionBBox.height();
    mTransformationBuffer->fill( Qt::transparent );

    delete mExtratedBuffer;
    mExtratedBuffer = new QImage( bboxWidth, bboxHeight, QImage::Format_ARGB32_Premultiplied );

    const int bpl = mOriginalImage->bytesPerLine();
    const int bufferBpl = mExtratedBuffer->bytesPerLine();

    uchar* sourceData = mOriginalImage->bits();
    uchar* sourceScanline = sourceData;

    uchar* bufferData = mExtratedBuffer->bits();
    uchar* bufferScanline = bufferData;

    const int minX = selectionBBox.left();
    const int minY = selectionBBox.top();
    const int maxX = selectionBBox.right();
    const int maxY = selectionBBox.bottom();

    for( int y = minY; y <= maxY; ++y )
    {
        sourceScanline = sourceData + y * bpl + minX * 4;
        bufferScanline = bufferData + (y - minY) * bufferBpl;

        for( int x = minX; x <= maxX; ++x )
        {
            // Extract pixels to buffer, and clears them from original image
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;
        }
    }
}



void cSelection::TransformSelection( const QTransform& iTransfo, double iXScale, double iYScale )
{
    if( !mExtratedBuffer )
        return;

    QRect dirtyArea = GetTransformationBBox();

    mTransfoWidth = mExtratedBuffer->width() * iXScale;
    mTransfoHeight = mExtratedBuffer->height() * iYScale;
    mTransfoOffset = mSelectionBBox.topLeft() + QPointF( iTransfo.dx(), iTransfo.dy() );

    QImage scaledBuffer = mExtratedBuffer->scaled( mTransfoWidth, mTransfoHeight, Qt::AspectRatioMode::IgnoreAspectRatio, Qt::TransformationMode::FastTransformation );

    dirtyArea.united( GetTransformationBBox() );

    CopyImage( &scaledBuffer, mTransformationBuffer, QPoint( mTransfoOffset.x(), mTransfoOffset.y() ) );
    mAssociatedClip->DirtyArea( dirtyArea );
}


void
cSelection::CancelTransformation()
{
    CopyImage( mExtratedBuffer, mOriginalImage, mSelectionBBox.topLeft() );
}


void
cSelection::ApplyTransformation()
{
    CopyImage( mExtratedBuffer, mOriginalImage, mSelectionBBox.topLeft() );//TODO: need transfo for that (at least the translation i guess)
}


QImage*
cSelection::GetSelectionEdgeMask()
{
    return  mEdgeDetectedMaskImage;
}


QRect
cSelection::GetSelectionBBox() const
{
    return  mSelectionBBox;
}


void
cSelection::_FilterAlpha()
{
    unsigned int index = 0;
    uchar* data = mMaskImage->bits();
    uchar* finalData = mEdgeDetectedMaskImage->bits();

    int minX = -1;
    int maxX = -1;
    int minY = -1;
    int maxY = -1;

    int width = mMaskImage->width();
    int height = mMaskImage->height();

    for( unsigned int y = 0; y < height ; ++y )
    {
        for( unsigned int x = 0; x < width; ++x )
        {
            index = y * mMaskImage->bytesPerLine() + x * 4;

            uchar alpha = data[ index + 3 ];
            if( alpha > 126 )
            {
                finalData[ index ]   = 0;
                finalData[ index+1 ] = 0;
                finalData[ index+2 ] = 0;
                finalData[ index+3 ] = 255;

                if( x < minX || minX == -1 )
                    minX = x;

                if( x > maxX || maxX == -1 )
                    maxX = x;

                if( y < minY || minY == -1 )
                    minY = y;

                if( y > maxY || maxY == -1 )
                    maxY = y;
            }
            else
            {
                finalData[ index ]   = 0;
                finalData[ index+1 ] = 0;
                finalData[ index+2 ] = 0;
                finalData[ index+3 ] = 0;
            }
        }
    }

    mSelectionBBox.setTopLeft( QPoint( minX, minY ) );
    mSelectionBBox.setBottomRight( QPoint( maxX, maxY ) ) ;
}


