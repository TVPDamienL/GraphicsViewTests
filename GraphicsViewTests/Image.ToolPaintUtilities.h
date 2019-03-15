#pragma once

#include "Image.Utilities.h"
#include "Thread.h"
#include "ThreadMachine.h"

#include <QRect>
#include <QTransform>
#include <QPolygon>

#include "ImageDebugger.h"



static
void
MTBlendImageNormalF( float* source, const int iSourceWidth, const int iSourceHeight,
                     float* destination, const int iDestWidth, const int iDestHeight,
                     QImage* iParallelRender,
                     const QPoint& point )
{
    float* sourceData = source;
    const int sourceBPL = iSourceWidth * 4;

    float* destData = destination;
    const int dstBPL = iDestWidth * 4;

    uchar* parallelData = iParallelRender->bits();
    const int parallelBPL = iParallelRender->bytesPerLine();

    // Source bbox
    const int minX = point.x();
    const int maxX = minX + iSourceWidth - 1;
    const int minY = point.y();
    const int maxY = minY + iSourceHeight - 1;

    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= iDestWidth ? iDestWidth - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= iDestHeight ? iDestHeight - 1 : maxY;
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
            [ sourceData, destData, parallelData, sourceBPL, dstBPL, parallelBPL, minX, minY ]( cRange iOff, cRange iRange )
        {
            float* sourceScanline = sourceData;
            float* destScanline = destData;
            uchar* parallelScanline = parallelData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                sourceScanline  = sourceData + (y - minY) * sourceBPL + (startX - minX) * 4;
                destScanline    = destData + y * dstBPL + startX * 4;
                parallelScanline    = parallelData + y * parallelBPL + startX * 4;

                for( int x = startX; x < endX; ++x )
                {
                    float alpha = *(sourceScanline + 3);
                    if( alpha == 0 ) // Skip if alpha is nil
                    {
                        sourceScanline += 4;
                        parallelScanline += 4;
                        destScanline += 4;
                        continue;
                    }

                    float transparencyAmountInverse = (255.F - alpha) / 255.F;

                    *destScanline = *sourceScanline + *destScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = *sourceScanline + *destScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = *sourceScanline + *destScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = alpha + *destScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++parallelScanline;
                    ++sourceScanline;
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


// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
static
void
MTDownscaleBoxAverageDirectAlphaF( float* iInput, const int iInputWidth, const int iInputHeight,
                                   float* iOutput, const int iOutputWidth, const int iOutputHeight,
                                   QImage* iParallelRender,
                                   float* iAlphaMask,  const int iAlphaWidth, const int iAlphaHeight,
                                   const QTransform& iTransform, const QPoint& iOrigin )
{
    const QTransform inverse = iTransform.inverted();
    const int inputWidth = iInputWidth;
    const int inputHeight = iInputHeight;
    QRect inputArea = QRect( 0, 0, iInputWidth, iInputHeight );
    inputArea.moveTopLeft( iOrigin );

    // Transformed bbox
    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );
    QRectF transfoBBox = ExclusiveBoundingBoxF( outputRect );

    int minX = transfoBBox.left();
    int minY = transfoBBox.top();

    transfoBBox = transfoBBox.intersected( QRect( 0, 0, iOutputWidth, iOutputHeight ) ); // Intersected here will be inclusive, so intersection with 1080 and 1086 = 1080 for x2

    int startingX = transfoBBox.left();
    int startingY = transfoBBox.top();
    int endingX = transfoBBox.right() >= iOutputWidth ? iOutputWidth - 1 : transfoBBox.right();
    int endingY = transfoBBox.bottom() >= iOutputHeight ? iOutputHeight - 1 : transfoBBox.bottom();

    // Scales
    const double xScaleFactor = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] ) / double( inputArea.width() );
    const double yScaleFactor = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] ) / double( inputArea.height() );

    if( xScaleFactor >= 1.0 || yScaleFactor >= 1.0 )
    {
        MTBlendImageNormalF( iInput, iInputWidth, iInputHeight, iOutput, iOutputWidth, iOutputHeight, iParallelRender, QPoint( minX, minY ) );
        return;
    }

    //const double xScaleInverse = 1/ xScaleFactor;
    //const double yScaleInverse = 1/ yScaleFactor;

    //// Data iteration
    //uchar* inputData = iInput->bits();
    //const int inputBPL = iInput->bytesPerLine();

    //uchar* outputData = iOutput->bits();
    //const int outputBPL = iOutput->bytesPerLine();

    ////uchar* alphaData = iAlphaMask->bits();
    ////uchar* alphaScanline = alphaData + 3;
    ////const int alphaBPL = iAlphaMask->bytesPerLine();


    //const int height = endingY - startingY;

    //const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    //const int split = height / threadCount;
    //const int excess = height % threadCount;

    //std::vector< cThreadHandle > handles;

    //for( int i = 0; i < threadCount; ++i )
    //{
    //    int correct = 0;
    //    if( i == threadCount-1 )
    //        correct = excess;

    //    handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
    //        [ = ]( cRange iOff, cRange iRange )
    //    {
    //        uchar* outputScanline;
    //        uchar* inputScanline;

    //        const int startY = iOff.mY;
    //        const int endY = startY + iRange.mY;
    //        const int startX = iOff.mX;
    //        const int endX = startX + iRange.mX;
    //        const int xOffset = startX * 4;

    //        // Pixel averaging variables
    //        unsigned int rSum = 0;
    //        unsigned int gSum = 0;
    //        unsigned int bSum = 0;
    //        unsigned int aSum = 0;
    //        double surface = 0.0;
    //        double xRatio = 1.0;
    //        double yRatio = 1.0;
    //        double finalRatio = 1.0;

    //        for( int y = startY; y < endY; ++y )
    //        {
    //            outputScanline = outputData + y * outputBPL + xOffset;
    //            //alphaScanline = alphaData +  (y-minY) * alphaBPL + xOffset + 3;


    //            for( int x = startX; x < endX; ++x )
    //            {
    //                // Get the point in original
    //                const QPointF xyMappedF = inverse.map( QPointF( x, y ) );
    //                const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );

    //                if( !inputArea.contains( xyMapped ) )
    //                {
    //                    continue;
    //                }

    //                // Get the box to read in original
    //                QRectF boxAreaF( xyMapped.x(), xyMapped.y(), xScaleInverse, yScaleInverse );
    //                QRect boxArea( xyMapped.x(), xyMapped.y(), int( xScaleInverse ) + 1, int( yScaleInverse ) + 1 );

    //                boxArea = boxArea.intersected( inputArea );

    //                if( !boxArea.isEmpty() )
    //                {
    //                    // Sum of all pixel values
    //                    for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
    //                    {
    //                        double ratio = 1 - (boxAreaF.top() - j);
    //                        if( ratio < 1.0 )
    //                        {
    //                            yRatio = ratio;
    //                        }
    //                        else
    //                        {
    //                            yRatio = Min( 1 - (j - boxAreaF.bottom()), 1.0);
    //                        }


    //                        inputScanline = inputData + j * inputBPL + boxArea.left() * 4;
    //                        for( int i = boxArea.left(); i <= boxArea.right(); ++i )
    //                        {
    //                            ratio = 1 - (boxAreaF.left() - i);
    //                            if( ratio < 1.0 )
    //                            {
    //                                xRatio = ratio;
    //                            }
    //                            else
    //                            {
    //                                xRatio = Min( 1 - (i - boxAreaF.right()), 1.0 );
    //                            }

    //                            finalRatio = xRatio * yRatio;

    //                            bSum += *inputScanline * finalRatio; ++inputScanline;
    //                            gSum += *inputScanline * finalRatio; ++inputScanline;
    //                            rSum += *inputScanline * finalRatio; ++inputScanline;
    //                            aSum += *inputScanline * finalRatio; ++inputScanline;

    //                            surface += finalRatio;
    //                        }
    //                    }

    //                    rSum /= surface;
    //                    gSum /= surface;
    //                    bSum /= surface;
    //                    aSum /= surface;

    //                    // Blend
    //                    BlendPixelNormal( &outputScanline, rSum, gSum, bSum, aSum );

    //                    rSum = 0;
    //                    gSum = 0;
    //                    bSum = 0;
    //                    aSum = 0;
    //                    surface = 0.0;
    //                }
    //            }
    //        }
    //    },
    //        cRange( startingX, startingY + i * split ), cRange( endingX - startingX + 1, split + correct ), true ) );
    //}

    //for( int i = 0; i < handles.size(); ++i )
    //{
    //    auto handle = handles[ i ];
    //    cThread* t = handle.GetThread();
    //    if( t )
    //        t->WaitEndOfTask();
    //}
}





static
void
TransformNearestNeighbourDirectOutputNormalBlendAlphaFParallel( const float* source, const int iSourceWidth, const int iSourceHeight,
                                                                float* destination, const int iDestWidth, const int iDestHeight,
                                                                QImage* iParallelRender,
                                                                const QTransform& iTransform, const QPoint& iOrigin, const QImage* iAlphaMask )
{
    const QTransform    inverse = iTransform.inverted();
    const int           inputWidth = iSourceWidth;
    const int           inputHeight = iSourceHeight;
    QRect               inputArea = QRect( 0, 0, iSourceWidth, iSourceHeight );
    inputArea.moveTopLeft( iOrigin );

    const int           outputWidth = iDestWidth;
    const int           outputHeight = iDestHeight;

    QPolygonF           outputRect = MapToPolygonF( iTransform, inputArea );
    QRect transfoBBox = ExclusiveBoundingBox( outputRect );

    int minX = transfoBBox.left();
    int minY = transfoBBox.top();
    int maxX = transfoBBox.right();
    int maxY = transfoBBox.bottom();

    transfoBBox = transfoBBox.intersected( QRect( 0, 0, iDestWidth, iDestHeight ) );
    // If QRect is 0, 0, 1920, 1080, it'll go from 0 to 1919. That's how Qt implements it
    // So by doing intersection between the output and anything else, we'll have x1/x2 - y1/y2 values between 0 and width/height-1
    // Which means, in the loop below, we go to <= endX/Y, and not <

    int startX = transfoBBox.left();
    int startY = transfoBBox.top();
    int endX = transfoBBox.right();
    int endY = transfoBBox.bottom();

    const float* inputData = source;
    const int inputBPL = iSourceWidth * 4;

    float* outputData = destination;
    float* outputScanline = outputData;
    const int outputBPL = iDestWidth * 4;

    const uchar* alphaData = iAlphaMask->bits();
    const uchar* alphaScanline = alphaData + 3;
    const int alphaBPL = iAlphaMask->bytesPerLine();

    uchar* parallelData = iParallelRender->bits();
    uchar* parallelScanline = parallelData;
    const int parallelBPL = iParallelRender->bytesPerLine();

    const int xOffset = startX * 4;
    float* scanXOffset = outputData + xOffset;

    for( int y = startY; y <= endY; ++y )
    {
        outputScanline = scanXOffset + y * outputBPL;
        alphaScanline = alphaData +  y * alphaBPL + startX * 4 + 3;

        for( int x = startX; x <= endX; ++x )
        {
            const QPoint xyMapped = inverse.map( QPoint( x, y ) );

            if( !inputArea.contains( xyMapped ) )
            {
                BlendPixelNormalFParrallel( &outputScanline, &parallelScanline, 0, 0, 0, 0 );
                continue;
            }

            int inputIndex = (xyMapped.y() - iOrigin.y()) * inputBPL + (xyMapped.x() - iOrigin.x()) * 4;

            float alphaMaskTransparency = *alphaScanline / 255.F; alphaScanline += 4;
            float r = inputData[ inputIndex + 2 ] * alphaMaskTransparency;
            float g = inputData[ inputIndex + 1 ] * alphaMaskTransparency;
            float b = inputData[ inputIndex + 0 ] * alphaMaskTransparency;
            float a = inputData[ inputIndex + 3 ] * alphaMaskTransparency;

            BlendPixelNormalFParrallel( &outputScanline, &parallelScanline, r, g, b, a );
        }
    }
}




static
void
TransformNearestNeighbourDirectOutputNormalBlendFParallel( const float* source, const int iSourceWidth, const int iSourceHeight,
                                                                float* destination, const int iDestWidth, const int iDestHeight,
                                                                QImage* iParallelRender,
                                                                const QTransform& iTransform, const QPoint& iOrigin )
{
    const QTransform    inverse = iTransform.inverted();
    const int           inputWidth = iSourceWidth;
    const int           inputHeight = iSourceHeight;
    QRect               inputArea = QRect( 0, 0, iSourceWidth, iSourceHeight );
    inputArea.moveTopLeft( iOrigin );

    const int           outputWidth = iDestWidth;
    const int           outputHeight = iDestHeight;

    QPolygonF           outputRect = MapToPolygonF( iTransform, inputArea );
    QRect transfoBBox = ExclusiveBoundingBox( outputRect );

    int minX = transfoBBox.left();
    int minY = transfoBBox.top();
    int maxX = transfoBBox.right();
    int maxY = transfoBBox.bottom();

    transfoBBox = transfoBBox.intersected( QRect( 0, 0, iDestWidth, iDestHeight ) );
    // If QRect is 0, 0, 1920, 1080, it'll go from 0 to 1919. That's how Qt implements it
    // So by doing intersection between the output and anything else, we'll have x1/x2 - y1/y2 values between 0 and width/height-1
    // Which means, in the loop below, we go to <= endX/Y, and not <

    int startX = transfoBBox.left();
    int startY = transfoBBox.top();
    int endX = transfoBBox.right();
    int endY = transfoBBox.bottom();

    const float* inputData = source;
    const int inputBPL = iSourceWidth * 4;

    float* outputData = destination;
    float* outputScanline = outputData;
    const int outputBPL = iDestWidth * 4;

    uchar* parallelData = iParallelRender->bits();
    uchar* parallelScanline = parallelData;
    const int parallelBPL = iParallelRender->bytesPerLine();

    const int xOffset = startX * 4;
    float* scanXOffset = outputData + xOffset;
    uchar* pScanXOffset = parallelData + xOffset;

    for( int y = startY; y <= endY; ++y )
    {
        outputScanline = scanXOffset + y * outputBPL;
        parallelScanline = pScanXOffset + y * parallelBPL;

        for( int x = startX; x <= endX; ++x )
        {
            const QPoint xyMapped = inverse.map( QPoint( x, y ) );

            if( !inputArea.contains( xyMapped ) )
            {
                BlendPixelNormalFParrallel( &outputScanline, &parallelScanline, 0, 0, 0, 0 );
                continue;
            }

            int inputIndex = (xyMapped.y() - iOrigin.y()) * inputBPL + (xyMapped.x() - iOrigin.x()) * 4;

            float r = inputData[ inputIndex + 2 ];
            float g = inputData[ inputIndex + 1 ];
            float b = inputData[ inputIndex + 0 ];
            float a = inputData[ inputIndex + 3 ];

            BlendPixelNormalFParrallel( &outputScanline, &parallelScanline, r, g, b, a );
        }
    }

    //IMAGEDEBUG->ShowImage( source, iSourceWidth, iSourceHeight );
}

