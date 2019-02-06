#pragma once

#include <QImage>

#include "Blending.h"



// Sets area with color
// This is like a memset
static
void
HardFill( QImage* dest, const QRect& area, const QColor& color )
{
    uchar* destData = dest->bits();
    uchar* destScanline = destData;
    int dstBPL = dest->bytesPerLine();

    const int startingX = area.left() < 0 ? 0 : area.left();
    const int endingX = area.right() > dest->width() - 1 ? dest->width() - 1 : area.right();
    const int startingY = area.top() < 0 ? 0 : area.top();
    const int endingY = area.bottom() > dest->height() - 1 ? dest->height() - 1 : area.bottom();

    const uint8_t r = color.red();
    const uint8_t g = color.green();
    const uint8_t b = color.blue();
    const uint8_t alpha = color.alpha();

    for( int y = startingY; y <= endingY; ++y )
    {
        destScanline    = destData + y * dstBPL + startingX * 4;

        for( int x = startingX; x <= endingX; ++x )
        {
            BlendPixelNone( &destScanline, r, g, b, alpha );
        }
    }
}




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
            BlendPixelNone( &destScanline, *(sourceScanline+2), *(sourceScanline+1), *(sourceScanline), *(sourceScanline+3) );
            sourceScanline += 4;
        }
    }
}



static
void
BlendImageNormal( QImage* source, QImage* destination, const QPoint& point )
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
            uchar alpha = *(sourceScanline+3);
            if( alpha == 0 ) // Skip if alpha is nil
            {
                sourceScanline += 4;
                destScanline += 4;
                continue;
            }

            BlendPixelNormal( &destScanline, *(sourceScanline+2), *(sourceScanline+1), *(sourceScanline), alpha );
            sourceScanline += 4;
        }
    }
}


//================================================================
//================================================================
//================================================================


// Assumes source && dest have same size
static
void
BlendImageNormalSameSizes( QImage* source, QImage* destination, const QRect& area )
{
    uchar* sourceData = source->bits();
    uchar* sourceScanline;
    int sourceBPL = source->bytesPerLine();

    uchar* destData = destination->bits();
    uchar* destScanline;
    int dstBPL = destination->bytesPerLine();

    assert(sourceBPL == dstBPL);
    assert(source->size() == destination->size() );

    // area
    const int minX = area.left();
    const int maxX = area.right();
    const int minY = area.top();
    const int maxY = area.bottom();

    // Clipping
    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= destination->width() ? destination->width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= destination->height() ? destination->height() - 1 : maxY;

    for( int y = startingY; y <= endingY; ++y )
    {
        const int index = y * sourceBPL + startingX * 4;
        sourceScanline  = sourceData + index;
        destScanline    = destData + index;

        for( int x = startingX; x <= endingX; ++x )
        {
            uchar alpha = *(sourceScanline+3);
            if( alpha == 0 ) // Skip if alpha is nil
            {
                sourceScanline += 4;
                destScanline += 4;
                continue;
            }

            BlendPixelNormal( &destScanline, *(sourceScanline+2), *(sourceScanline+1), *(sourceScanline), alpha );
            sourceScanline += 4;
        }
    }
}
