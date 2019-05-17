#pragma once

#include <QImage>

#include "Blending.h"
#include "BenchmarkStuff.h"

#include "Math.Fast.h"
#include "ThreadMachine.h"
#include "Thread.h"

#include "Image.Utilities.h"


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
    const int height = endingY - startingY + 1;

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
                                                                             // +1 because we range over height amount
                                                                                // It then works with endX being startY + iRange.mY;
                                                                                // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
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
    const int height = endingY - startingY + 1; // +1 because rect.right used for endingX is width - 1, so 299 for a 300 wide rectangle

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
                                                                             // +1 because we range over height amount
                                                                                // It then works with endX being startY + iRange.mY;
                                                                                // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}


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
    const int height = endingY - startingY + 1;

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

            for( int y = startY; y < endY; ++y )
            {
                sourceScanline  = sourceData + (y - minY) * sourceBPL + (startX - minX) * 4;
                destScanline    = destData + y * dstBPL + startX * 4;

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
                                                                             // +1 because we range over height amount
                                                                                // It then works with endX being startY + iRange.mY;
                                                                                // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}


static
void
MTBlendImagesF( const float* source, const int sourceW, const int sourceH, const QRect& iSourceArea,
                float* destination, const int dstW, const int dstH, const QPoint& point,
                const float* alphaMask, const int alphaW, const int alphaH, const QPoint& alphaPoint,
                float iOpacity )
{
    int sourceBPL = sourceW*4;
    int dstBPL = dstW*4;
    int alphaBPL = alphaW;

    const QPoint offset = point - iSourceArea.topLeft();
    const QPoint offsetAlpha = alphaPoint - iSourceArea.topLeft();

    QRect srcArea( 0, 0, sourceW, sourceH );
    QRect dstArea( 0, 0, dstW, dstH );
    QRect alphaArea( 0, 0, alphaW, alphaH );

    // Clip to src rect
    QRect workingArea = srcArea.intersected( iSourceArea );

    // Clip to dst rect
    workingArea = workingArea.translated( offset );
    workingArea = workingArea.intersected( dstArea );
    workingArea = workingArea.translated( -offset );

    // Clip to alpha rect
    workingArea = workingArea.translated( offsetAlpha );
    workingArea = workingArea.intersected( alphaArea );
    workingArea = workingArea.translated( -offsetAlpha );

    const int startingX   = workingArea.left();
    const int endingX     = workingArea.right();
    const int startingY   = workingArea.top();
    const int endingY     = workingArea.bottom();

    int height = endingY - startingY + 1;

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
            const float* sourceScanline = source;
            float* destScanline = destination;
            //const float* alphaScan = alphaMask;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                sourceScanline  = source + y * sourceBPL + startX * 4;
                destScanline    = destination + (y + offset.y()) * dstBPL + (startX + offset.x()) * 4;
                //alphaScan    = alphaMask + (y + offsetAlpha.y()) * alphaBPL + (startX + offsetAlpha.x());

                for( int x = startX; x < endX; ++x )
                {
                    float alpha = *(sourceScanline+3);
                    if( alpha == 0 ) // Skip if alpha is nil
                    {
                        sourceScanline += 4;
                        destScanline += 4;
                        //++alphaScan;
                        continue;
                    }

                    // This would allow to pick color using the tip shape, but it leaves a lot of mistakes, and i dunno if it's subpixel related atm
                    float v = /**alphaScan / 255.F **/ iOpacity;

                    BlendPixelNormalF( &destScanline, *(sourceScanline+2) * v, *(sourceScanline+1) * v, *(sourceScanline) * v, alpha * v );
                    sourceScanline += 4;
                    //++alphaScan;
                }
            }
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
        // +1 because we range over height amount
        // It then works with endX being startY + iRange.mY;
        // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
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
    const int height = endingY - startingY + 1;

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
                                                                             // +1 because we range over height amount
                                                                                // It then works with endX being startY + iRange.mY;
                                                                                // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
    }

    for( int i = 0; i < handles.size(); ++i )
    {
        auto handle = handles[ i ];
        cThread* t = handle.GetThread();
        if( t )
            t->WaitEndOfTask();
    }
}




// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
// This one swipes the average area over, plus area isn't rounded left and top
static
void
MTDownscaleBoxAverageDirectAlpha2( QImage* iInput, QImage* iOutput, QImage* iAlphaMask, const QTransform& iTransform, const QPoint& iOrigin )
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

    transfoBBox = transfoBBox.intersected( iOutput->rect() );

    int startingX = transfoBBox.left();
    int startingY = transfoBBox.top();
    int endingX = transfoBBox.right() >= iOutput->width() ? iOutput->width() - 1 : transfoBBox.right();
    int endingY = transfoBBox.bottom() >= iOutput->height() ? iOutput->height() - 1 : transfoBBox.bottom();

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



    const int height = endingY - startingY + 1;

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

            QRectF avgArea( startX, startY, iInput->width() * xScaleFactor, iInput->height() * yScaleFactor );

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
                    avgArea.moveTopLeft( xyMappedF );
                    QRect boxArea( xyMapped.x(), xyMapped.y(), int( xScaleInverse ) + 1, int( yScaleInverse ) + 1 );

                    boxArea = boxArea.intersected( inputArea );

                    if( !boxArea.isEmpty() )
                    {
                        // Sum of all pixel values
                        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
                        {
                            double ratio = 1 - (avgArea.top() - j);
                            if( ratio < 1.0 )
                            {
                                yRatio = ratio;
                            }
                            else
                            {
                                yRatio = Min( 1 - (j - avgArea.bottom()), 1.0);
                            }


                            inputScanline = inputData + j * inputBPL + boxArea.left() * 4;
                            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
                            {
                                ratio = 1 - (avgArea.left() - i);
                                if( ratio < 1.0 )
                                {
                                    xRatio = ratio;
                                }
                                else
                                {
                                    xRatio = Min( 1 - (i - avgArea.right()), 1.0 );
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


