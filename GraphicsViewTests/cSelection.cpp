#include "cSelection.h"


cSelection::~cSelection()
{
}


cSelection::cSelection( int iWidth, int iHeight )
{
    mMaskImage              = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    mEdgeDetectedMaskImage  = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    mSelectedBuffer         = new QImage( iWidth, iHeight, QImage::Format_RGBA8888_Premultiplied );
    Clear();
}


QImage*
cSelection::GetSelectionMask()
{
    return  mMaskImage;
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
    EmitValueChanged( kBoundsChanged );
}


void
cSelection::ExtractPixelsToBuffer( QImage * iImage )
{
    delete mTransformBuffer;
    const int imageWidth = iImage->width();
    const int imageHeight = iImage->height();

    mTransformBuffer = new QImage( imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied );

    const int bpl = iImage->bytesPerLine();
    const int bufferBpl = mTransformBuffer->bytesPerLine();
    const QRect selectionBBox = GetSelectionBBox();

    uchar* sourceData = iImage->bits();
    uchar* sourceScanline = sourceData;

    uchar* bufferData = mTransformBuffer->bits();
    uchar* bufferScanline = bufferData;

    for( int y = selectionBBox.top(); y < selectionBBox.bottom(); ++y )
    {
        sourceScanline = sourceData + y * bpl + selectionBBox.left() * 4;
        bufferScanline = bufferData + y * bufferBpl;

        for( int x = selectionBBox.left(); x < selectionBBox.right(); ++x )
        {
            *bufferScanline = *sourceScanline; ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;
            *bufferScanline = *sourceScanline; ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;
            *bufferScanline = *sourceScanline; ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;
            *bufferScanline = *sourceScanline; ++bufferScanline;
            *sourceScanline = 0; ++sourceScanline;
        }
    }
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


