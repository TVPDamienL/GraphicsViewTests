#pragma once

#include <QImage>

#include "Blending.h"
#include "BenchmarkStuff.h"

static uint counterFill = 0;
static uint counterCopy = 0;

static
void
Reset__()
{
    counterFill = 0;
    counterCopy = 0;
}

static
void
ShowMe__()
{
    qDebug() << "Fill : " << counterFill << " -- Copy : " << counterCopy;
}



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
            ++counterFill;
        }
    }
}


// Blends area with color
static
void
NormalFill( QImage* dest, const QRect& area, const QColor& color )
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
            BlendPixelNormal( &destScanline, r, g, b, alpha );
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
    const int endingX = maxX > destination->width() - 1 ? destination->width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY > destination->height() - 1 ? destination->height() - 1 : maxY;

    for( int y = startingY; y <= endingY; ++y )
    {
        sourceScanline  = sourceData + (y - minY) * sourceBPL + (startingX - minX) * 4;
        destScanline    = destData + y * dstBPL + startingX * 4;

        for( int x = startingX; x <= endingX; ++x )
        {
            BlendPixelNone( &destScanline, *(sourceScanline+2), *(sourceScanline+1), *(sourceScanline), *(sourceScanline+3) );
            sourceScanline += 4;
            ++counterCopy;
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


// ==============================================================================
// ==============================================================================
// ==============================================================================


static
QPolygonF
MapToPolygonF( const QTransform& iTransfo, const QRectF& iRect )
{
    QPolygonF output;

    output.append( iTransfo.map( iRect.topLeft() ) );
    output.append( iTransfo.map( iRect  .topRight() ) );
    output.append( iTransfo.map( iRect  .bottomRight() ) );
    output.append( iTransfo.map( iRect  .bottomLeft() ) );

    return  output;
}


static
QPolygonF
MapToPolygonF( const QTransform& iTransfo, const QRect& iRect )
{
    return  MapToPolygonF( iTransfo, QRectF( iRect ) );
}


//================================================================
//================================================================
//================================================================


static
QImage*
TransformNearestNeighbour( QImage* iInput, /*QImage* iOutput, */const QTransform& iTransform )
{
    QImage* output = 0;

    const QTransform inverse = iTransform.inverted();
    const int inputWidth = iInput->width();
    const int inputHeight = iInput->height();

    QPolygonF outputRect = MapToPolygonF( iTransform, iInput->rect() );

    // Transformed BBox
    int minX = outputRect.at( 0 ).x();
    int minY = outputRect.at( 0 ).y();
    int maxX = outputRect.at( 0 ).x();
    int maxY = outputRect.at( 0 ).y();

    for( auto point : outputRect )
    {
        if( point.x() < minX )
            minX = point.x();

        if( point.y() < minY )
            minY = point.y();

        if( point.x() > maxX )
            maxX = point.x();

        if( point.y() > maxY )
            maxY = point.y();
    }

    QRect transfoBBox( minX, minY, maxX - minX, maxY - minY );

    output = new QImage( transfoBBox.width(), transfoBBox.height(), QImage::Format::Format_ARGB32_Premultiplied );

    uchar* inputData = iInput->bits();
    const int inputBPL = iInput->bytesPerLine();

    uchar* outputData = output->bits();
    uchar* outputScanline = outputData;
    const int outputBPL = output->bytesPerLine();


    for( int y = minY; y < maxY; ++y )
    {
        outputScanline = outputData + (y-minY) * outputBPL;

        for( int x = minX; x < maxX; ++x )
        {
            const QPointF xyMapped = inverse.map( QPointF( x, y ) );
            const QPoint xyMappedCut( xyMapped.x(), xyMapped.y() );

            if( xyMapped.x() < 0 || xyMapped.y() < 0
                || xyMapped.x() > inputWidth -1 || xyMapped.y() > inputHeight -1 )
            {
                *outputScanline = 0; ++outputScanline;
                *outputScanline = 0; ++outputScanline;
                *outputScanline = 0; ++outputScanline;
                *outputScanline = 0; ++outputScanline;
                continue;
            }

            int inputIndex = xyMappedCut.y() * inputBPL + xyMappedCut.x() * 4;
            *outputScanline = inputData[ inputIndex + 0 ]; ++outputScanline;
            *outputScanline = inputData[ inputIndex + 1 ]; ++outputScanline;
            *outputScanline = inputData[ inputIndex + 2 ]; ++outputScanline;
            *outputScanline = inputData[ inputIndex + 3 ]; ++outputScanline;
        }
    }

    return  output;
}



