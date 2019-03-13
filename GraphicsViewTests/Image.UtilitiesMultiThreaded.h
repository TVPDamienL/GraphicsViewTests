#pragma once

#include <QImage>

#include "Blending.h"
#include "BenchmarkStuff.h"

#include "Math.Fast.h"
#include "ThreadMachine.h"
#include "Thread.h"


// Sets area with color
// This is like a memset
static
void
MTHardFill( QImage* dest, const QRect& area, const QColor& color )
{
    uchar* destData = dest->bits();
    int dstBPL = dest->bytesPerLine();

    const int startingX = area.left() < 0 ? 0 : area.left();
    const int endingX = area.right() > dest->width() - 1 ? dest->width() - 1 : area.right();
    const int startingY = area.top() < 0 ? 0 : area.top();
    const int endingY = area.bottom() > dest->height() - 1 ? dest->height() - 1 : area.bottom();
    const int height = endingY - startingY;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ color, destData, dstBPL ]( cRange iOff, cRange iRange )
        {
            uchar* destScanline = destData;

            const uint8_t r = color.red();
            const uint8_t g = color.green();
            const uint8_t b = color.blue();
            const uint8_t alpha = color.alpha();

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                destScanline = destData + y * dstBPL + startX * 4;

                for( int x = startX; x < endX; ++x )
                {
                    BlendPixelNone( &destScanline, r, g, b, alpha );
                }
            }
        },
        cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}


// Sets area with color
// This is like a memset
static
void
MTHardFillF( float* dest, const int iWidth, const int iHeight, const QRect& area, const QColor& color )
{
    float* destData = dest;
    int dstBPL = iWidth * 4;

    const int startingX = area.left() < 0 ? 0 : area.left();
    const int endingX = area.right() > iWidth - 1 ? iWidth - 1 : area.right();
    const int startingY = area.top() < 0 ? 0 : area.top();
    const int endingY = area.bottom() > iHeight - 1 ? iHeight - 1 : area.bottom();
    const int height = endingY - startingY;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ color, destData, dstBPL ]( cRange iOff, cRange iRange )
        {
            float* destScanline = destData;

            const float r = color.red();
            const float g = color.green();
            const float b = color.blue();
            const float alpha = color.alpha();

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                destScanline = destData + y * dstBPL + startX * 4;

                for( int x = startX; x < endX; ++x )
                {
                    BlendPixelNoneF( &destScanline, r, g, b, alpha );
                }
            }
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}

//
//// Blends area with color
//static
//void
//NormalFill( QImage* dest, const QRect& area, const QColor& color )
//{
//    uchar* destData = dest->bits();
//    uchar* destScanline = destData;
//    int dstBPL = dest->bytesPerLine();
//
//    const int startingX = area.left() < 0 ? 0 : area.left();
//    const int endingX = area.right() > dest->width() - 1 ? dest->width() - 1 : area.right();
//    const int startingY = area.top() < 0 ? 0 : area.top();
//    const int endingY = area.bottom() > dest->height() - 1 ? dest->height() - 1 : area.bottom();
//
//    const uint8_t r = color.red();
//    const uint8_t g = color.green();
//    const uint8_t b = color.blue();
//    const uint8_t alpha = color.alpha();
//
//    for( int y = startingY; y <= endingY; ++y )
//    {
//        destScanline    = destData + y * dstBPL + startingX * 4;
//
//        for( int x = startingX; x <= endingX; ++x )
//        {
//            BlendPixelNormal( &destScanline, r, g, b, alpha );
//        }
//    }
//}
//
//
//
//
//// Copies source to destination at point
//static
//void
//CopyImage( QImage* source, QImage* destination, const QPoint& point )
//{
//    uchar* sourceData = source->bits();
//    uchar* sourceScanline;
//    int sourceBPL = source->bytesPerLine();
//
//    uchar* destData = destination->bits();
//    uchar* destScanline;
//    int dstBPL = destination->bytesPerLine();
//
//    // Source bbox
//    const int minX = point.x();
//    const int maxX = minX + source->width() - 1;
//    const int minY = point.y();
//    const int maxY = minY + source->height() - 1;
//
//    const int startingX = minX < 0 ? 0 : minX;
//    const int endingX = maxX > destination->width() - 1 ? destination->width() - 1 : maxX;
//    const int startingY = minY < 0 ? 0 : minY;
//    const int endingY = maxY > destination->height() - 1 ? destination->height() - 1 : maxY;
//
//    for( int y = startingY; y <= endingY; ++y )
//    {
//        sourceScanline  = sourceData + (y - minY) * sourceBPL + (startingX - minX) * 4;
//        destScanline    = destData + y * dstBPL + startingX * 4;
//
//        for( int x = startingX; x <= endingX; ++x )
//        {
//            BlendPixelNone( &destScanline, *(sourceScanline+2), *(sourceScanline+1), *(sourceScanline), *(sourceScanline+3) );
//            sourceScanline += 4;
//        }
//    }
//}


static
void
MTBlendImageNormal( QImage* source, QImage* destination, const QPoint& point )
{
    uchar* sourceData = source->bits();
    int sourceBPL = source->bytesPerLine();

    uchar* destData = destination->bits();
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
    const int height = endingY - startingY;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ sourceData, destData, sourceBPL, dstBPL, minX, minY ]( cRange iOff, cRange iRange )
        {
            uchar* sourceScanline = sourceData;
            uchar* destScanline = destData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y <= endY; ++y )
            {
                sourceScanline  = sourceData + (y - minY) * sourceBPL + (startX - minX) * 4;
                destScanline    = destData + y * dstBPL + startX * 4;

                for( int x = startX; x <= endX; ++x )
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
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX, split + correct ), true ) );
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}


//================================================================
//================================================================
//================================================================


// Assumes source && dest have same size
static
void
MTBlendImageNormalSameSizes( QImage* source, QImage* destination, const QRect& area )
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
    const int height = endingY - startingY;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ sourceData, destData, sourceBPL, dstBPL ]( cRange iOff, cRange iRange )
        {
            uchar* destScanline;
            uchar* sourceScanline;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                const int index = y * sourceBPL + startX * 4;
                sourceScanline  = sourceData + index;
                destScanline    = destData + index;


                for( int x = startX; x < endX; ++x )
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
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}

//
//// ==============================================================================
//// ==============================================================================
//// ==============================================================================
//
//
//static
//QPolygonF
//MapToPolygonF( const QTransform& iTransfo, const QRectF& iRect )
//{
//    QPolygonF output;
//
//    output.append( iTransfo.map( iRect.topLeft() ) );
//    output.append( iTransfo.map( iRect  .topRight() ) );
//    output.append( iTransfo.map( iRect  .bottomRight() ) );
//    output.append( iTransfo.map( iRect  .bottomLeft() ) );
//
//    return  output;
//}
//
//
//static
//QPolygonF
//MapToPolygonF( const QTransform& iTransfo, const QRect& iRect )
//{
//    return  MapToPolygonF( iTransfo, QRectF( iRect ) );
//}
//
//
////================================================================
////================================================================
////================================================================
//
//
//static
//QImage*
//TransformNearestNeighbourIntoImage( QImage* iInput, const QTransform& iTransform )
//{
//    QImage* output = 0;
//
//    const QTransform inverse = iTransform.inverted();
//    const int inputWidth = iInput->width();
//    const int inputHeight = iInput->height();
//    const QRect inputArea = iInput->rect();
//
//    QPolygonF           outputRect = MapToPolygonF( iTransform, inputArea );
//    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
//
//    int minX = transfoBBox.left();
//    int minY = transfoBBox.top();
//    int maxX = transfoBBox.right();
//    int maxY = transfoBBox.bottom();
//
//    output = new QImage( transfoBBox.width(), transfoBBox.height(), QImage::Format::Format_ARGB32_Premultiplied );
//
//    uchar* inputData = iInput->bits();
//    const int inputBPL = iInput->bytesPerLine();
//
//    uchar* outputData = output->bits();
//    uchar* outputScanline = outputData;
//    const int outputBPL = output->bytesPerLine();
//
//
//    for( int y = minY; y <= maxY; ++y )
//    {
//        outputScanline = outputData + (y-minY) * outputBPL;
//
//        for( int x = minX; x <= maxX; ++x )
//        {
//            const QPoint xyMapped = inverse.map( QPoint( x, y ) );
//
//            if( !inputArea.contains( xyMapped ) )
//            {
//                BlendPixelNone( &outputScanline, 0, 0, 0, 0 );
//                continue;
//            }
//
//            int inputIndex = xyMapped.y() * inputBPL + xyMapped.x() * 4;
//            BlendPixelNone( &outputScanline, inputData[ inputIndex + 2 ], inputData[ inputIndex + 1 ], inputData[ inputIndex + 0 ], inputData[ inputIndex + 3 ] );
//        }
//    }
//
//    return  output;
//}
//
//
//static
//void
//TransformNearestNeighbourDirectOutput( QImage* iInput, QImage* iOutput, const QTransform& iTransform, const QPoint& iOrigin )
//{
//    const QTransform    inverse = iTransform.inverted();
//    const int           inputWidth = iInput->width();
//    const int           inputHeight = iInput->height();
//    QRect               inputArea = iInput->rect();
//    inputArea.moveTopLeft( iOrigin );
//
//    const int           outputWidth = iOutput->width();
//    const int           outputHeight = iOutput->height();
//
//    QPolygonF           outputRect = MapToPolygonF( iTransform, inputArea );
//    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
//
//    int minX = transfoBBox.left();
//    int minY = transfoBBox.top();
//    int maxX = transfoBBox.right();
//    int maxY = transfoBBox.bottom();
//
//    transfoBBox = transfoBBox.intersected( iOutput->rect() );
//    // If QRect is 0, 0, 1920, 1080, it'll go from 0 to 1919. That's how Qt implements it
//    // So by doing intersection between the output and anything else, we'll have x1/x2 - y1/y2 values between 0 and width/height-1
//    // Which means, in the loop below, we go to <= endX/Y, and not <
//
//    int startX = transfoBBox.left();
//    int startY = transfoBBox.top();
//    int endX = transfoBBox.right();
//    int endY = transfoBBox.bottom();
//
//    uchar* inputData = iInput->bits();
//    const int inputBPL = iInput->bytesPerLine();
//
//    uchar* outputData = iOutput->bits();
//    uchar* outputScanline = outputData;
//    const int outputBPL = iOutput->bytesPerLine();
//
//    const int xOffset = startX * 4;
//    uchar* scanXOffset = outputData + xOffset;
//
//    for( int y = startY; y <= endY; ++y )
//    {
//        outputScanline = scanXOffset + y * outputBPL;
//
//        for( int x = startX; x <= endX; ++x )
//        {
//            const QPoint xyMapped = inverse.map( QPoint( x, y ) );
//
//            if( !inputArea.contains( xyMapped ) )
//            {
//                BlendPixelNone( &outputScanline, 0, 0, 0, 0 );
//                continue;
//            }
//
//            int inputIndex = (xyMapped.y() - iOrigin.y()) * inputBPL + (xyMapped.x() - iOrigin.x()) * 4;
//            BlendPixelNone( &outputScanline, inputData[ inputIndex + 2 ], inputData[ inputIndex + 1 ], inputData[ inputIndex + 0 ], inputData[ inputIndex + 3 ] );
//        }
//    }
//}
//
//
//static
//void
//TransformNearestNeighbourDirectOutputNormalBlendAlpha( QImage* iInput, QImage* iOutput, const QTransform& iTransform, const QPoint& iOrigin, QImage* iAlphaMask )
//{
//    const QTransform    inverse = iTransform.inverted();
//    const int           inputWidth = iInput->width();
//    const int           inputHeight = iInput->height();
//    QRect               inputArea = iInput->rect();
//    inputArea.moveTopLeft( iOrigin );
//
//    const int           outputWidth = iOutput->width();
//    const int           outputHeight = iOutput->height();
//
//    QPolygonF           outputRect = MapToPolygonF( iTransform, inputArea );
//    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
//
//    int minX = transfoBBox.left();
//    int minY = transfoBBox.top();
//    int maxX = transfoBBox.right();
//    int maxY = transfoBBox.bottom();
//
//    transfoBBox = transfoBBox.intersected( iOutput->rect() );
//    // If QRect is 0, 0, 1920, 1080, it'll go from 0 to 1919. That's how Qt implements it
//    // So by doing intersection between the output and anything else, we'll have x1/x2 - y1/y2 values between 0 and width/height-1
//    // Which means, in the loop below, we go to <= endX/Y, and not <
//
//    int startX = transfoBBox.left();
//    int startY = transfoBBox.top();
//    int endX = transfoBBox.right();
//    int endY = transfoBBox.bottom();
//
//    uchar* inputData = iInput->bits();
//    const int inputBPL = iInput->bytesPerLine();
//
//    uchar* outputData = iOutput->bits();
//    uchar* outputScanline = outputData;
//    const int outputBPL = iOutput->bytesPerLine();
//
//    uchar* alphaData = iAlphaMask->bits();
//    uchar* alphaScanline = alphaData + 3;
//    const int alphaBPL = iAlphaMask->bytesPerLine();
//
//
//    const int xOffset = startX * 4;
//    uchar* scanXOffset = outputData + xOffset;
//
//    for( int y = startY; y <= endY; ++y )
//    {
//        outputScanline = scanXOffset + y * outputBPL;
//        alphaScanline = alphaData +  y * alphaBPL + startX * 4 + 3;
//
//        for( int x = startX; x <= endX; ++x )
//        {
//            const QPoint xyMapped = inverse.map( QPoint( x, y ) );
//
//            if( !inputArea.contains( xyMapped ) )
//            {
//                BlendPixelNormal( &outputScanline, 0, 0, 0, 0 );
//                continue;
//            }
//
//            int inputIndex = (xyMapped.y() - iOrigin.y()) * inputBPL + (xyMapped.x() - iOrigin.x()) * 4;
//
//            uchar r = inputData[ inputIndex + 2 ];
//            uchar g = inputData[ inputIndex + 1 ];
//            uchar b = inputData[ inputIndex + 0 ];
//            uchar a = inputData[ inputIndex + 3 ];
//
//            int alphaMaskTransparency = *alphaScanline; alphaScanline += 4;
//            r = BlinnMult( r, alphaMaskTransparency );
//            g = BlinnMult( g, alphaMaskTransparency );
//            b = BlinnMult( b, alphaMaskTransparency );
//            a = BlinnMult( a, alphaMaskTransparency );
//
//            BlendPixelNormal( &outputScanline, r, g, b, a );
//        }
//    }
//}
//
//
//static
//QImage*
//TransformBilinearIntoImage( QImage* iInput, const QTransform& iTransform )
//{
//    QImage* output = 0;
//
//    const QTransform inverse = iTransform.inverted();
//    const int inputWidth = iInput->width();
//    const int inputHeight = iInput->height();
//    const QRect inputArea = iInput->rect();
//
//    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );
//    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
//
//    int minX = transfoBBox.left();
//    int minY = transfoBBox.top();
//    int maxX = transfoBBox.right();
//    int maxY = transfoBBox.bottom();
//
//    output = new QImage( transfoBBox.width(), transfoBBox.height(), QImage::Format::Format_ARGB32_Premultiplied );
//
//    uchar* inputData = iInput->bits();
//    const int inputBPL = iInput->bytesPerLine();
//
//    uchar* outputData = output->bits();
//    uchar* outputScanline = outputData;
//    const int outputBPL = output->bytesPerLine();
//
//
//    for( int y = minY; y < maxY; ++y )
//    {
//        outputScanline = outputData + (y-minY) * outputBPL;
//
//        for( int x = minX; x < maxX; ++x )
//        {
//            const QPoint xyMapped = inverse.map( QPoint( x, y ) );
//
//            if( !inputArea.contains( xyMapped ) )
//            {
//                BlendPixelNone( &outputScanline, 0, 0, 0, 0 );
//                continue;
//            }
//
//            // TODO
//
//            int inputIndex = xyMapped.y() * inputBPL + xyMapped.x() * 4;
//            BlendPixelNone( &outputScanline, inputData[ inputIndex + 2 ], inputData[ inputIndex + 1 ], inputData[ inputIndex + 0 ], inputData[ inputIndex + 3 ] );
//        }
//    }
//
//    return  output;
//}
//
//
//
//// iTransform should be a downscale, otherwise it's not ment to work
//// This averages pixels to get the condensed pixel
//static
//QImage*
//DownscaleBoxAverageIntoImage( QImage* iInput, const QTransform& iTransform )
//{
//    QImage* output = 0;
//
//    const QTransform inverse = iTransform.inverted();
//    const int inputWidth = iInput->width();
//    const int inputHeight = iInput->height();
//    const QRect inputArea = iInput->rect();
//
//    // Transformed bbox
//    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );
//    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
//
//    int minX = transfoBBox.left();
//    int minY = transfoBBox.top();
//    int maxX = transfoBBox.right() + 1;
//    int maxY = transfoBBox.bottom() + 1;
//
//    // Scales
//    const double xScaleFactor = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] ) / double( inputArea.width() );
//    const double yScaleFactor = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] ) / double( inputArea.height() );
//
//    if( xScaleFactor >= 1.0 || yScaleFactor >= 1.0 )
//        return  new  QImage( *iInput );
//
//    const double xScaleInverse = 1/ xScaleFactor;
//    const double yScaleInverse = 1/ yScaleFactor;
//
//    // The output image
//    output = new QImage( transfoBBox.width()+1, transfoBBox.height()+1, QImage::Format::Format_ARGB32_Premultiplied );
//
//    // Data iteration
//    uchar* inputData = iInput->bits();
//    const int inputBPL = iInput->bytesPerLine();
//    uchar* inputScanline = inputData;
//
//    uchar* outputData = output->bits();
//    uchar* outputScanline = outputData;
//    const int outputBPL = output->bytesPerLine();
//
//    // Pixel averaging variables
//    unsigned int rSum = 0;
//    unsigned int gSum = 0;
//    unsigned int bSum = 0;
//    unsigned int aSum = 0;
//    double surface = 0.0;
//    double xRatio = 1.0;
//    double yRatio = 1.0;
//    double finalRatio = 1.0;
//
//    for( int y = minY; y <= maxY; ++y )
//    {
//        outputScanline = outputData + (y-minY) * outputBPL;
//
//        for( int x = minX; x <= maxX; ++x )
//        {
//            // Get the point in original
//            const QPointF xyMappedF = inverse.map( QPointF( x, y ) );
//            const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );
//
//            if( !inputArea.contains( xyMapped ) )
//            {
//                BlendPixelNone( &outputScanline, 0, 0, 0, 0 );
//                continue;
//            }
//
//            // Get the box to read in original
//            QRectF boxAreaF( xyMapped.x(), xyMapped.y(), xScaleInverse, yScaleInverse );
//            QRect boxArea( xyMapped.x(), xyMapped.y(), int( xScaleInverse ) + 1, int( yScaleInverse ) + 1 );
//
//
//            // Clip to not write oob
//            boxArea = boxArea.intersected( inputArea );
//
//            if( !boxArea.isEmpty() )
//            {
//                // Sum of all pixel values
//                for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
//                {
//                    double ratio = 1 - (boxAreaF.top() - j);
//                    if( ratio < 1.0 )
//                    {
//                        yRatio = ratio;
//                    }
//                    else
//                    {
//                        yRatio = Min( 1 - (j - boxAreaF.bottom()), 1.0);
//                    }
//
//
//                    inputScanline = inputData + j * inputBPL + boxArea.left() * 4;
//                    for( int i = boxArea.left(); i <= boxArea.right(); ++i )
//                    {
//                        ratio = 1 - (boxAreaF.left() - i);
//                        if( ratio < 1.0 )
//                        {
//                            xRatio = ratio;
//                        }
//                        else
//                        {
//                            xRatio = Min( 1 - (i - boxAreaF.right()), 1.0 );
//                        }
//
//                        finalRatio = xRatio * yRatio;
//
//                        bSum += *inputScanline * finalRatio; ++inputScanline;
//                        gSum += *inputScanline * finalRatio; ++inputScanline;
//                        rSum += *inputScanline * finalRatio; ++inputScanline;
//                        aSum += *inputScanline * finalRatio; ++inputScanline;
//
//                        surface += finalRatio;
//                    }
//                }
//
//                rSum /= surface;
//                gSum /= surface;
//                bSum /= surface;
//                aSum /= surface;
//
//                // Blend
//                BlendPixelNone( &outputScanline, rSum, gSum, bSum, aSum );
//
//                rSum = 0;
//                gSum = 0;
//                bSum = 0;
//                aSum = 0;
//                surface = 0.0;
//            }
//        }
//    }
//
//    return  output;
//}
//
//


// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
static
void
MTDownscaleBoxAverageDirectAlpha( QImage* iInput, QImage* iOutput, QImage* iAlphaMask, const QTransform& iTransform, const QPoint& iOrigin )
{
    const QTransform inverse = iTransform.inverted();
    const int inputWidth = iInput->width();
    const int inputHeight = iInput->height();
    QRect inputArea = iInput->rect();
    inputArea.moveTopLeft( iOrigin );

    // Transformed bbox
    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );
    QRectF transfoBBox = ExclusiveBoundingBoxF( outputRect );

    int minX = transfoBBox.left();
    int minY = transfoBBox.top();
    int maxX = transfoBBox.right() + 1;
    int maxY = transfoBBox.bottom() + 1;

    transfoBBox = transfoBBox.intersected( iOutput->rect() );

    int startingX = transfoBBox.left();
    int startingY = transfoBBox.top();
    int endingX = transfoBBox.right();
    int endingY = transfoBBox.bottom();

    // Scales
    const double xScaleFactor = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] ) / double( inputArea.width() );
    const double yScaleFactor = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] ) / double( inputArea.height() );

    if( xScaleFactor >= 1.0 || yScaleFactor >= 1.0 )
    {
        MTBlendImageNormal( iInput, iOutput, QPoint( minX, minY ) );
        return;
    }

    const double xScaleInverse = 1/ xScaleFactor;
    const double yScaleInverse = 1/ yScaleFactor;

    // Data iteration
    uchar* inputData = iInput->bits();
    const int inputBPL = iInput->bytesPerLine();

    uchar* outputData = iOutput->bits();
    const int outputBPL = iOutput->bytesPerLine();

    //uchar* alphaData = iAlphaMask->bits();
    //uchar* alphaScanline = alphaData + 3;
    //const int alphaBPL = iAlphaMask->bytesPerLine();


    const int height = endingY - startingY;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ = ]( cRange iOff, cRange iRange )
        {
            uchar* outputScanline;
            uchar* inputScanline;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;
            const int xOffset = startX * 4;

            // Pixel averaging variables
            unsigned int rSum = 0;
            unsigned int gSum = 0;
            unsigned int bSum = 0;
            unsigned int aSum = 0;
            double surface = 0.0;
            double xRatio = 1.0;
            double yRatio = 1.0;
            double finalRatio = 1.0;

            for( int y = startY; y < endY; ++y )
            {
                outputScanline = outputData + y * outputBPL + xOffset;
                //alphaScanline = alphaData +  (y-minY) * alphaBPL + xOffset + 3;


                for( int x = startX; x < endX; ++x )
                {
                    // Get the point in original
                    const QPointF xyMappedF = inverse.map( QPointF( x, y ) );
                    const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );

                    if( !inputArea.contains( xyMapped ) )
                    {
                        continue;
                    }

                    // Get the box to read in original
                    QRectF boxAreaF( xyMapped.x(), xyMapped.y(), xScaleInverse, yScaleInverse );
                    QRect boxArea( xyMapped.x(), xyMapped.y(), int( xScaleInverse ) + 1, int( yScaleInverse ) + 1 );

                    boxArea = boxArea.intersected( inputArea );

                    if( !boxArea.isEmpty() )
                    {
                        // Sum of all pixel values
                        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
                        {
                            double ratio = 1 - (boxAreaF.top() - j);
                            if( ratio < 1.0 )
                            {
                                yRatio = ratio;
                            }
                            else
                            {
                                yRatio = Min( 1 - (j - boxAreaF.bottom()), 1.0);
                            }


                            inputScanline = inputData + j * inputBPL + boxArea.left() * 4;
                            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
                            {
                                ratio = 1 - (boxAreaF.left() - i);
                                if( ratio < 1.0 )
                                {
                                    xRatio = ratio;
                                }
                                else
                                {
                                    xRatio = Min( 1 - (i - boxAreaF.right()), 1.0 );
                                }

                                finalRatio = xRatio * yRatio;

                                bSum += *inputScanline * finalRatio; ++inputScanline;
                                gSum += *inputScanline * finalRatio; ++inputScanline;
                                rSum += *inputScanline * finalRatio; ++inputScanline;
                                aSum += *inputScanline * finalRatio; ++inputScanline;

                                surface += finalRatio;
                            }
                        }

                        rSum /= surface;
                        gSum /= surface;
                        bSum /= surface;
                        aSum /= surface;

                        // Blend
                        BlendPixelNormal( &outputScanline, rSum, gSum, bSum, aSum );

                        rSum = 0;
                        gSum = 0;
                        bSum = 0;
                        aSum = 0;
                        surface = 0.0;
                    }
                }
            }
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}





