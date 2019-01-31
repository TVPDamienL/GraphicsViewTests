#pragma once

#include <QImage>



// Copies source to destination at point
static
void
CopyImage( QImage* source, QImage* destination, const QPoint& point )
{
    uchar* sourceData = source->bits();
    uchar* sourceScanline;
    int sourceBPL = source->bytesPerLine();

    uchar* destData = destination->bits();
    uchar* destScanline;
    int dstBPL = destination->bytesPerLine();

    // Source bbox
    const int minX = point.x();
    const int maxX = minX + source->width() - 1;
    const int minY = point.y();
    const int maxY = minY + source->height() - 1;

    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= destination->width() ? destination->width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= destination->height() ? destination->height() - 1 : maxY;


    for( int y = startingY; y <= endingY; ++y )
    {
        sourceScanline  = sourceData + (y - minY) * sourceBPL + (startingX - minX) * 4;
        destScanline    = destData + y * dstBPL + startingX * 4;

        for( int x = startingX; x <= endingX; ++x )
        {
            // BGRA order
            *(destScanline + 2) = *sourceScanline; ++sourceScanline;
            *(destScanline + 1) = *sourceScanline; ++sourceScanline;
            *(destScanline    ) = *sourceScanline; ++sourceScanline;
            *(destScanline + 3) = *sourceScanline; ++sourceScanline;

            destScanline += 4;
        }
    }
}



static
void
BlendImage( QImage* source, QImage* destination, const QPoint& point )
{
    //uchar* sourceData = source->bits();
    //uchar* sourceScanline;
    //int sourceBPL = source->bytesPerLine();

    //uchar* destData = destination->bits();
    //uchar* destScanline;
    //int dstBPL = destination->bytesPerLine();

    //// Source bbox
    //const int minX = point.x();
    //const int maxX = minX + source->width() - 1;
    //const int minY = point.y();
    //const int maxY = minY + source->height() - 1;

    //const int startingX = minX < 0 ? 0 : minX;
    //const int endingX = maxX >= destination->width() ? destination->width() - 1 : maxX;
    //const int startingY = minY < 0 ? 0 : minY;
    //const int endingY = maxY >= destination->height() ? destination->height() - 1 : maxY;


    //for( unsigned int y = minY; y < maxY ; ++y )
    //{
    //    sourceScanline  = sourceData + (y - minY) * sourceBPL + (startingX - minX) * 4;
    //    destScanline    = destData + y * dstBPL + startingX * 4;

    //    for( unsigned int x = minX; x < maxX; ++x )
    //    {
    //        uchar sourceAlpha = originPixelRow[ 3 ];
    //        if( sourceAlpha == 0 )
    //        {
    //            originPixelRow += 4;
    //            pixelRow += 4;
    //            continue;
    //        }

    //        BlendPixelNormal( &pixelRow, *(originPixelRow+2), *(originPixelRow+1), *(originPixelRow), *(originPixelRow+3) ); originPixelRow += 4;
    //    }
    //}
}