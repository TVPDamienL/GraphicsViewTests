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
    mTransformationBBox = QRectF( 0, 0, 0, 0 );
}


void
cSelection::ProcessEdgeDetection()
{
    _FilterAlpha();
    mEdgeDetectedMaskImage = mEdgeDetectionConvolution.GetEdgeDectionFromImage( mEdgeDetectedMaskImage );
}


void
cSelection::ExtractPixelsFromImageToBuffer()
{
    const QRect selectionBBox = GetSelectionBBox();
    const int bboxWidth = mOriginalSelectionBBox.width();
    const int bboxHeight = mOriginalSelectionBBox.height();

    mTransformationBuffer->fill( Qt::transparent );


    delete mExtratedBuffer;
    mExtratedBuffer = new QImage( bboxWidth, bboxHeight, QImage::Format_ARGB32_Premultiplied );

    const int bpl = mOriginalImage->bytesPerLine();
    const int bufferBpl = mExtratedBuffer->bytesPerLine();

    // Source we extract from
    uchar* sourceData = mOriginalImage->bits();
    uchar* sourceScanline = sourceData;

    // The selection mask to know if we extract pixel or not
    uchar* maskData = mMaskImage->bits();
    uchar* maskScanline = maskData;

    // The output extracted buffer image
    uchar* bufferData = mExtratedBuffer->bits();
    uchar* bufferScanline = bufferData;

    const int minX = selectionBBox.left();
    const int minY = selectionBBox.top();
    const int maxX = selectionBBox.right();
    const int maxY = selectionBBox.bottom();

    for( int y = minY; y <= maxY; ++y )
    {
        const int sourceAndAlphaIndex = y * bpl + minX * 4;
        sourceScanline = sourceData + sourceAndAlphaIndex;
        maskScanline = maskData + sourceAndAlphaIndex + 3; // +3 to read alpha
        bufferScanline = bufferData + (y - minY) * bufferBpl;

        for( int x = minX; x <= maxX; ++x )
        {
            if( *maskScanline == 0 )
            {
                sourceScanline += 4;
                maskScanline += 4;

                *bufferScanline = 0; ++bufferScanline;
                *bufferScanline = 0; ++bufferScanline;
                *bufferScanline = 0; ++bufferScanline;
                *bufferScanline = 0; ++bufferScanline;

                continue;
            }

            // Extract pixels to buffer, and clears them from original image
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0;++sourceScanline;
            *bufferScanline = *sourceScanline;      ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;

            maskScanline += 4;
        }
    }

    // Copies the extracted buffer into the transformation buffer to start with
    CopyImage( mExtratedBuffer, mTransformationBuffer, selectionBBox.topLeft() );
    mTransformationBBox = selectionBBox;
}



void cSelection::TransformSelection( const QTransform& iTransfo, const QRectF& iTransformedBBox )
{
    if( !mExtratedBuffer )
        return;

    QRect dirtyArea = GetTransformationBBox();

    mTransformationBBox = iTransformedBBox;
    mTransformationBBox.setTopLeft( mOriginalSelectionBBox.topLeft() + QPointF( iTransfo.dx(), iTransfo.dy() ) );

    auto transfoWithoutTranslatioon = QTransform();
    transfoWithoutTranslatioon.setMatrix( iTransfo.m11(), iTransfo.m12(), iTransfo.m13(), iTransfo.m21(), iTransfo.m22(), iTransfo.m23(), 0, 0, 1 );

    QImage scaledBuffer = mExtratedBuffer->transformed( transfoWithoutTranslatioon );

    //dirtyArea = dirtyArea.united( GetTransformationBBox() );

    CopyImage( &scaledBuffer, mTransformationBuffer, GetTransformationBBox().topLeft() );
    mAssociatedClip->DirtyArea( dirtyArea );
}


void
cSelection::CancelTransformation()
{
    QRect dirtyArea = GetTransformationBBox();
    BlendImageNormal( mExtratedBuffer, mOriginalImage, mOriginalSelectionBBox.topLeft() );
    dirtyArea = dirtyArea.united( mOriginalSelectionBBox );

    Clear();
    mAssociatedClip->DirtyArea( dirtyArea );
}


void
cSelection::ApplyTransformation()
{
    BlendImageNormalSameSizes( mTransformationBuffer, mOriginalImage, GetTransformationBBox() );

    Clear();
    mAssociatedClip->DirtyArea( GetTransformationBBox() );
}


QImage*
cSelection::GetSelectionEdgeMask()
{
    return  mEdgeDetectedMaskImage;
}


QRect
cSelection::GetSelectionBBox() const
{
    return  mOriginalSelectionBBox;
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

    minX = minX == -1 ? 0 : minX;
    minY = minY == -1 ? 0 : minY;
    maxX = maxX == -1 ? 0 : maxX;
    maxY = maxY == -1 ? 0 : maxY;

    mOriginalSelectionBBox.setTopLeft( QPoint( minX, minY ) );
    mOriginalSelectionBBox.setBottomRight( QPoint( maxX, maxY ) ) ;
}


