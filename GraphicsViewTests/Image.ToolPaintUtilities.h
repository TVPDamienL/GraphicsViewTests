#pragma once

#include "Image.Utilities.h"
#include "Thread.h"
#include "ThreadMachine.h"

#include <QRect>
#include <QTransform>
#include <QPolygon>

#include "ImageDebugger.h"




static
inline
void
ReadImageWithFloatArea( const QPointF& iOffset, int iX, int iY, float iTopLeftRatio, float iTopRightRatio, float iBottomLeftRatio, float iBottomRightRatio,
                        const float* iImage, int iImageWidth, int iImageHeight,
                        float* oRed, float* oGreen, float *oBlue, float* oAlpha )
{

    const float* scanline;
    const int bpl = iImageWidth * 4;

    // Cuz int cut on negative value cuts up, down down : 1.2 -> 1 ==> Reduction :: -1.2 -> -1.0 ==> Increase
    int offX = iOffset.x();
    if( iOffset.x() < 0 )
        offX -= 1;

    int minX = iX + offX;

    // Cuz int cut on negative value cuts up, down down : 1.2 -> 1 ==> Reduction :: -1.2 -> -1.0 ==> Increase
    int offY = iOffset.y();
    if( iOffset.y() < 0 )
        offY -= 1;

    int minY = iY + offY;

    const int maxX = minX + 1;
    const int maxY = minY + 1;

    const int startingX = Clamp( minX, 0, iImageWidth - 1 );
    const int endingX   = Clamp( maxX, 0, iImageWidth - 1 );
    const int startingY = Clamp( minY, 0, iImageHeight - 1 );
    const int endingY   = Clamp( maxY, 0, iImageHeight - 1 );
    const int total     = (endingX - startingX + 1) * (endingY - startingY + 1);

    *oRed   = 0.F;
    *oGreen = 0.F;
    *oBlue  = 0.F;
    *oAlpha = 0.F;

    scanline = iImage + minY * bpl + minX * 4;
    // Top left
    if( minX >= 0 && minX < iImageWidth && minY >= 0 && minY < iImageHeight )
    {
        *oBlue  += *scanline * iTopLeftRatio; ++scanline;
        *oGreen += *scanline * iTopLeftRatio; ++scanline;
        *oRed   += *scanline * iTopLeftRatio; ++scanline;
        *oAlpha += *scanline * iTopLeftRatio; ++scanline;
    }
    else
    {
        scanline += 4;
    }

    // Top right
    if( maxX >= 0 && maxX < iImageWidth && minY >= 0 && minY < iImageHeight )
    {
        *oBlue  += *scanline * iTopRightRatio; ++scanline;
        *oGreen += *scanline * iTopRightRatio; ++scanline;
        *oRed   += *scanline * iTopRightRatio; ++scanline;
        *oAlpha += *scanline * iTopRightRatio; ++scanline;
    }
    else
    {
        scanline += 4;
    }


    scanline += bpl - 8;
    // Bottom left
    if( minX >= 0 && minX < iImageWidth && maxY >= 0 && maxY < iImageHeight )
    {
        *oBlue  += *scanline * iBottomLeftRatio; ++scanline;
        *oGreen += *scanline * iBottomLeftRatio; ++scanline;
        *oRed   += *scanline * iBottomLeftRatio; ++scanline;
        *oAlpha += *scanline * iBottomLeftRatio; ++scanline;
    }
    else
    {
        scanline += 4;
    }

    // Bottom Right
    if( maxX >= 0 && maxX < iImageWidth && maxY >= 0 && maxY < iImageHeight )
    {
        *oBlue  += *scanline * iBottomRightRatio; ++scanline;
        *oGreen += *scanline * iBottomRightRatio; ++scanline;
        *oRed   += *scanline * iBottomRightRatio; ++scanline;
        *oAlpha += *scanline * iBottomRightRatio; ++scanline;
    }
}


static
void
MTBlendImageNormalF( const float* source, const int iSourceWidth, const int iSourceHeight,  // Source we will blend
                     const float* background, const int iBGWidth, const int iBGHeight,      // The background over which we blend
                     float* destination, const int iDestWidth, const int iDestHeight,       // The output result buffer, can be the same as background
                     QImage* iParallelRender,
                     const QPoint& point )
{
    const float* sourceData = source;
    const int sourceBPL = iSourceWidth * 4;

    const float* bgData = background;
    const int bgBPL = iBGWidth * 4;

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
            [ = ]( cRange iOff, cRange iRange )
        {
            const float* sourceScanline = sourceData;
            const float* bgScanline = bgData;
            float* destScanline = destData;
            uchar* parallelScanline = parallelData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;
            const int xOffset = startX * 4;

            for( int y = startY; y < endY; ++y )
            {
                sourceScanline  = sourceData + (y - minY) * sourceBPL + (startX - minX) * 4;
                bgScanline  = bgData + y * bgBPL + xOffset;
                destScanline    = destData + y * dstBPL + xOffset;
                parallelScanline    = parallelData + y * parallelBPL + xOffset;

                for( int x = startX; x < endX; ++x )
                {
                    float alpha = *(sourceScanline + 3);
                    if( alpha == 0 ) // Skip if alpha is nil
                    {
                        sourceScanline += 4;
                        bgScanline += 4;
                        parallelScanline += 4;
                        destScanline += 4;
                        continue;
                    }

                    float transparencyAmountInverse = (255.F - alpha) / 255.F;

                    *destScanline = *sourceScanline + *bgScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++bgScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = *sourceScanline + *bgScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++bgScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = *sourceScanline + *bgScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++bgScanline;
                    ++parallelScanline;
                    ++sourceScanline;

                    *destScanline = alpha + *bgScanline * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++bgScanline;
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






static
void
MTBlendImageNormalFDry( const float* source, const int iSourceWidth, const int iSourceHeight,  // Source we will blend
                        const float* drybuffer, const int iWidth, const int iHeight,      // The background over which we blend
                        float* stampbuffer,

                        float* destination,         // The float output result buffer
                        QImage* iParallelRender,    // The 8bit output buffer
                        const QPointF& point,
                        const float maxAlpha )
{
    const uchar maxAlphaRanged = maxAlpha * 255.F;
    const int sourceBPL = iSourceWidth * 4;
    const int buffersBPL = iWidth * 4;

    uchar* parallelData = iParallelRender->bits();
    const int parallelBPL = iParallelRender->bytesPerLine();

    // Final area in float
    QRectF floatArea( point.x(), point.y(), iSourceWidth, iSourceHeight );
    const QPointF subReadOffset = -(point - QPoint( point.x(), point.y() )); // Basically the offset representing the amount that has been cut by int

    // Keep offset between 0 and 1
    // TODO: if offset is larger than 1 or -1, need to do multiple +1 or -1 : 2.16 -> 0.16
    // noneed: indeed sudReadBuffer is the subtraction between a flot and its int cut, which can't be > 1
    QPointF offsetFinal = subReadOffset;
    if( offsetFinal.x() < 0 )
        offsetFinal.setX( offsetFinal.x() + 1 );
    if( offsetFinal.y() < 0 )
        offsetFinal.setY( offsetFinal.y() + 1 );

    float topLeftRatio      = (1-offsetFinal.y())   * (1-offsetFinal.x());
    float topRightRatio     = (1-offsetFinal.y())   * offsetFinal.x();
    float bottomRightRatio  = offsetFinal.y()       * offsetFinal.x();
    float bottomLeftRatio   = offsetFinal.y()       * (1-offsetFinal.x());

    // Source bbox
    const int minX = point.x();
    const int maxX = minX + iSourceWidth; // Max is not min + width -1 here because we want to expand the int area by one, to handle float area entirely
    const int minY = point.y();
    const int maxY = minY + iSourceHeight; // Max is not min + width -1 here because we want to expand the int area by one, to handle float area entirely


    // Clipped bounds == iteration limits
    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= iWidth ? iWidth - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= iHeight ? iHeight - 1 : maxY;
    const int height = endingY - startingY + 1;

    const int threadCount = cThreadProcessor::Instance()->GetAvailableThreadCount();
    const int split = height / threadCount;
    const int excess = height % threadCount;

    std::vector< cThreadHandle > handles;

    for( int i = 0; i < threadCount; ++i )
    {
        int correct = 0;
        if( split == 0 )
            i = threadCount-1;

        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ = ]( cRange iOff, cRange iRange )
        {
            const float* sourceScanline = source;
            const float* dryScan = drybuffer;
            float* stampScan = stampbuffer;
            float* destScanline = destination;
            uchar* parallelScanline = parallelData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;
            const int xOffset = startX * 4;

            float yRatio = 1.F;
            float xRatio = 1.F;
            float pixelRatio = 1.F;

            for( int y = startY; y < endY; ++y )
            {
                // Indexes and stuff
                sourceScanline  = source + (y - minY) * sourceBPL + (startX - minX) * 4;
                const int indexOffset = y * buffersBPL + xOffset;
                dryScan  = drybuffer + indexOffset;
                stampScan = stampbuffer + indexOffset;
                destScanline    = destination + indexOffset;
                parallelScanline    = parallelData + y * parallelBPL + xOffset;

                for( int x = startX; x < endX; ++x )
                {
                    float red2, green2, blue2, alpha2;
                    ReadImageWithFloatArea( subReadOffset, (x - minX), (y - minY), topLeftRatio, topRightRatio, bottomLeftRatio, bottomRightRatio,
                                            source, iSourceWidth, iSourceHeight,
                                            &red2, &green2, &blue2, &alpha2 );


                    float alpha = *(sourceScanline + 3);
                    if( alpha2 == 0 ) // Skip if alpha is nil
                    {
                        sourceScanline += 4;
                        dryScan += 4;
                        stampScan += 4;
                        destScanline += 4;
                        parallelScanline += 4;
                        continue;
                    }

                    const float inverseCeiled = (1 - (alpha2 / maxAlphaRanged)) * pixelRatio;

                    float* gPtr = stampScan + 1;
                    float* rPtr = stampScan + 2;
                    float* alphaPtr = stampScan + 3;

                    *stampScan  = blue2 + *stampScan  * inverseCeiled; ++sourceScanline;
                    *gPtr       = green2 + *gPtr       * inverseCeiled; ++sourceScanline;
                    *rPtr       = red2 + *rPtr       * inverseCeiled; ++sourceScanline;
                    *alphaPtr   = alpha2 + *alphaPtr   * inverseCeiled; ++sourceScanline;

                    const float transparencyAmountInverse = 1 - *alphaPtr / 255.F;

                    *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;
                    ++stampScan;

                    *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;
                    ++stampScan;

                    *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;
                    ++stampScan;

                    *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;
                    ++stampScan;
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
MTDownscaleBoxAverageDirectAlphaF( const float* iInput, const int iInputWidth, const int iInputHeight,  // Source we will blend
                                   const float* background, const int iBGWidth, const int iBGHeight,    // The background over which we blend
                                   float* iOutput, const int iOutputWidth, const int iOutputHeight,     // The output result buffer, can be the same as background
                                   QImage* iParallelRender,                                             // The uchar buffer we wanna write to at the same time
                                   float* iAlphaMask,  const int iAlphaWidth, const int iAlphaHeight,   // An alpha mask to apply to the blend
                                   const QTransform& iTransform, const QPoint& iOrigin )                // Transform and its origin to apply
{
    QTransform transfoFinal = iTransform;


    const int inputWidth = iInputWidth;
    const int inputHeight = iInputHeight;
    QRect inputArea = QRect( 0, 0, iInputWidth, iInputHeight );
    inputArea.moveTopLeft( iOrigin );

    // Transformed bbox
    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );
    for( auto& point : outputRect )
    {
        point.setX( std::round( point.x() ) );
        point.setY( std::round( point.y() ) );
    }

    QRect transfoBBox = ExclusiveBoundingBox( outputRect );

    int minX = transfoBBox.left();
    int minY = transfoBBox.top();

    transfoBBox = transfoBBox.intersected( QRect( 0, 0, iOutputWidth, iOutputHeight ) ); // Intersected here will be inclusive, so intersection with 1080 and 1086 = 1080 for x2

    int startingX = transfoBBox.left();
    int startingY = transfoBBox.top();
    int endingX = transfoBBox.right() >= iOutputWidth ? iOutputWidth - 1 : transfoBBox.right();
    int endingY = transfoBBox.bottom() >= iOutputHeight ? iOutputHeight - 1 : transfoBBox.bottom();

    auto check = endingX - startingX + 1;
    if( check % 2 == 0 )
        int bp = 5;

    // Scales
    const float transfoWidth = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] );
    const float transfoHeight = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] );

    const double xScaleFactorOriginal = transfoWidth / double( inputArea.width() );
    const double yScaleFactorOriginal = transfoHeight / double( inputArea.height() );
    const double xScaleFactorRounded = int(transfoWidth) / double( inputArea.width() );
    const double yScaleFactorRounded = int(transfoHeight) / double( inputArea.height() );

    if( xScaleFactorOriginal >= 1.0 || yScaleFactorOriginal >= 1.0 )
    {
        MTBlendImageNormalF( iInput, iInputWidth, iInputHeight, background, iBGWidth, iBGHeight, iOutput, iOutputWidth, iOutputHeight, iParallelRender, QPoint( minX, minY ) );
        return;
    }

    // We decide to work with ratios resulting from the rounded result
    // Any scale would provide a floating output size, let's say 4.48 for example
    // Final result will be 4 pixels wide, not 4.48, as we need int values.
    // Now, dividing the image in 4.48 will result in missing datas (because for loop below will go 4 iterations, not 4.48,, and 4*X < 4.48*X),
    // and thus, uneven results at lower sizes
    // So we take the final int size, compute the scale this represents, and set this one as the transformation scale, so we can evenly cover the original image
    transfoFinal = QTransform::fromScale( 1/xScaleFactorOriginal, 1/yScaleFactorOriginal ) * QTransform::fromScale( xScaleFactorRounded, yScaleFactorRounded ) * transfoFinal;
    QTransform inverse = transfoFinal.inverted();


    //const double xScaleInverse = 1/ xScaleFactor;
    //const double yScaleInverse = 1/ yScaleFactor;

    const float floatBoxWidth = float(inputArea.width()) / float(transfoWidth);
    const float floatBoxHeight = float(inputArea.height()) / float(transfoHeight);

    // Data iteration
    const float * inputData = iInput;
    const int inputBPL = iInputWidth * 4;

    float* outputData = iOutput;
    const int outputBPL = iOutputWidth * 4;

    uchar* parallelData = iParallelRender->bits();
    const int parallelBPL = iParallelRender->bytesPerLine();

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
        if( split == 0 )
            i = threadCount-1;

        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ = ]( cRange iOff, cRange iRange )
        {
            float* outputScanline;
            const float* inputScanline;
            uchar* parallelRender;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY - 1; // -1 because offset is 0 based, and range is how many time you do stuff
                                                    // So, range 25 = do 25 times ( 0 to 24 --> range - 1 )
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX - 1;
            const int xOffset = startX * 4;

            // Pixel averaging variables
            float rSum = 0;
            float gSum = 0;
            float bSum = 0;
            float aSum = 0;
            float surface = 0.0;
            float xRatio = 1.0;
            float yRatio = 1.0;
            float finalRatio = 1.0;

            for( int y = startY; y <= endY; ++y )
            {
                outputScanline = outputData + y * outputBPL + xOffset;
                parallelRender = parallelData + y * parallelBPL + xOffset;
                //alphaScanline = alphaData +  (y-minY) * alphaBPL + xOffset + 3;


                for( int x = startX; x <= endX; ++x )
                {
                    // Get the point in original
                    const QPointF xyMappedF = inverse.map( QPointF( x, y ) ); // To use this wee need to tune the transformation with new scale
                    const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );

                    if( !inputArea.contains( xyMapped ) )
                    {
                        continue;
                    }

                    // Get the box to read in original
                    QRectF boxAreaF( xyMappedF.x(), xyMappedF.y(), floatBoxWidth, floatBoxHeight );
                    QRect boxArea( xyMapped.x(), xyMapped.y(), int( floatBoxWidth ) + 1, int( floatBoxHeight ) + 1 );

                    boxArea = boxArea.intersected( inputArea );

                    if( !boxArea.isEmpty() )
                    {
                        // Sum of all pixel values
                        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
                        {
                            float ratio = 1 - (boxAreaF.top() - j);
                            if( ratio < 1.0 )
                            {
                                yRatio = ratio;
                            }
                            else
                            {
                                yRatio = std::min( 1.F - (float(j+1) - float(boxAreaF.bottom())), 1.F);
                            }


                            inputScanline = inputData + j * inputBPL + boxArea.left() * 4;
                            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
                            {
                                ratio = 1.F - (boxAreaF.left() - i);
                                if( ratio < 1.0 )
                                {
                                    xRatio = ratio;
                                }
                                else
                                {
                                    xRatio = std::min( 1.F - (float(i+1) - float(boxAreaF.right())), 1.F );
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
                        BlendPixelNormalFParrallel( &outputScanline, &parallelRender, rSum, gSum, bSum, aSum );

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




// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================




// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
static
void                                                                // All float images MUST be the same size, this allows faster access
MTDownscaleBoxAverageDirectAlphaFDry( const float* iInput, const int iWidth, const int iHeight,     // Source we will blend

                                    const float* background, const int iOutputBuffersWidth, const int iOutputBuffersHeight,    // The background over which we blend
                                    float* stampBuffer,                                              // The buffer holding the current tool line

                                    float* iOutput,                                                  // The output result buffer, can be the same as background
                                    QImage* iParallelRender,                                             // The uchar buffer we wanna write to at the same time
                                    float* iAlphaMask,  const int iAlphaWidth, const int iAlphaHeight,   // An alpha mask to apply to the blend
                                    const QTransform& iTransform, const QPoint& iOrigin, // Transform and its origin to apply
                                    const QPointF& iSubPixelOffset,
                                    const float maxalpha )
{
    QTransform transfoFinal = iTransform;

    const int inputWidth = iWidth;
    const int inputHeight = iHeight;
    QRect inputArea = QRect( 0, 0, iWidth, iHeight );
    inputArea.moveTopLeft( iOrigin );
    QRectF inputAreaF = QRect( 0, 0, iWidth, iHeight );
    inputAreaF.translate( iSubPixelOffset );

    // Transformed bbox
    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea ); // Apply transfo over inputArea not F, because transfo holds the offset already

    QRect transfoBBoxI = ExclusiveBoundingBox( outputRect );
    QRectF transfoBBoxF = ExclusiveBoundingBoxF( outputRect );
    const QPointF subPixelPos = transfoBBoxF.topLeft();
    const QPointF subpixelOffset = -(subPixelPos - QPoint( subPixelPos.x(), subPixelPos.y() )); // Basically the offset representing the amount that has been cut by int

    // Add one because float will go from 0.2 to 1.2 for example
    // So we need to cover pixels from 0 to 2
    // 0 is the min being int cut, 2 is max being int cut + 1
    transfoBBoxI.setWidth( transfoBBoxI.width() + 1 );
    transfoBBoxI.setHeight( transfoBBoxI.height() + 1 );

    int minX = transfoBBoxI.left();
    int minY = transfoBBoxI.top();

    transfoBBoxI = transfoBBoxI.intersected( QRect( 0, 0, iOutputBuffersWidth, iOutputBuffersHeight ) ); // Intersected here will be inclusive, so intersection with 1080 and 1086 = 1080 for x2

    int startingX = transfoBBoxI.left();
    int startingY = transfoBBoxI.top();
    int endingX = transfoBBoxI.right() >= iOutputBuffersWidth ? iOutputBuffersWidth - 1 : transfoBBoxI.right();
    int endingY = transfoBBoxI.bottom() >= iOutputBuffersHeight ? iOutputBuffersHeight - 1 : transfoBBoxI.bottom();

    //qDebug() << " W1 : " << transfoBBoxF.width();

    // Scales
    const float transfoWidth = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] );
    const float transfoHeight = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] );
    //qDebug() << " W2 : " << transfoWidth;

    const double xScaleFactorOriginal = transfoWidth /      inputAreaF.width() ;
    const double yScaleFactorOriginal = transfoHeight /     inputAreaF.height();
    const double xScaleFactorRounded = int(transfoWidth) /  inputAreaF.width() ;
    const double yScaleFactorRounded = int(transfoHeight) / inputAreaF.height();

    if( xScaleFactorOriginal >= 1.0 || yScaleFactorOriginal >= 1.0 )
    {
        MTBlendImageNormalFDry( iInput, iWidth, iHeight,
                                background, iOutputBuffersWidth, iOutputBuffersHeight, stampBuffer, iOutput, iParallelRender,
                                subPixelPos, maxalpha );
        return;
    }


    // We decide to work with ratios resulting from the rounded result
    // Any scale would provide a floating output size, let's say 4.48 for example
    // Final result will be 4 pixels wide, not 4.48, as we need int values.
    // Now, dividing the image in 4.48 will result in missing datas (because for loop below will go 4 iterations, not 4.48,, and 4*X < 4.48*X),
    // and thus, uneven results at lower sizes
    // So we take the final int size, compute the scale this represents, and set this one as the transformation scale, so we can evenly cover the original image
    transfoFinal = QTransform::fromScale( 1/xScaleFactorOriginal, 1/yScaleFactorOriginal ) * QTransform::fromScale( xScaleFactorRounded, yScaleFactorRounded ) * transfoFinal;
    QTransform inverse = transfoFinal.inverted();

    const float floatBoxWidth = inputAreaF.width() / transfoWidth;
    const float floatBoxHeight = inputAreaF.height() / transfoHeight;


    // Data iteration
    const float * inputData = iInput;
    float* outputData = iOutput;
    uchar* parallelData = iParallelRender->bits();

    const int sourceBPL = iWidth * 4;
    const int buffersBPL = iOutputBuffersWidth * 4;
    const int parallelBPL = iParallelRender->bytesPerLine();

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
        if( split == 0 )
            i = threadCount-1;

        int correct = 0;
        if( i == threadCount-1 )
            correct = excess;

        handles.push_back( cThreadProcessor::Instance()->AffectFunctionToThreadAndStart(
            [ = ]( cRange iOff, cRange iRange )
        {
            const uchar maxAlphaRanged = maxalpha * 255.F;
            const float* sourceScanline = iInput;
            const float* dryScan = background;
            float* stampScan = stampBuffer;
            float* destScanline = iOutput;
            uchar* parallelScanline = parallelData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY - 1; // -1 because offset is 0 based, and range is how many time you do stuff
                                                     // So, range 25 = do 25 times ( 0 to 24 --> range - 1 )
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX - 1;
            const int xOffset = startX * 4;

            // Pixel averaging variables
            float rSum = 0;
            float gSum = 0;
            float bSum = 0;
            float aSum = 0;
            float surface = 0.0;
            float xRatio = 1.0;
            float yRatio = 1.0;
            float finalRatio = 1.0;

            for( int y = startY; y <= endY; ++y )
            {
                sourceScanline  = iInput + (y - minY) * sourceBPL + (startX - minX) * 4;
                const int indexOffset = y * buffersBPL + xOffset;
                dryScan  = background + indexOffset;
                stampScan = stampBuffer + indexOffset;
                destScanline    = iOutput + indexOffset;
                parallelScanline    = parallelData + y * parallelBPL + xOffset;
                //alphaScanline = alphaData +  (y-minY) * alphaBPL + xOffset + 3;

                for( int x = startX; x <= endX; ++x )
                {
                    // Get the point in original
                    const QPointF xyMappedF = inverse.map( QPointF( x, y ) ); // To use this wee need to tune the transformation with new scale
                    const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );

                    if( !inputAreaF.contains( xyMapped ) )
                    {
                        continue;
                    }

                    // Get the box to read in original
                    QRectF boxAreaF( xyMappedF.x(), xyMappedF.y(), floatBoxWidth, floatBoxHeight );
                    //QRect boxArea( boxAreaF.left(), boxAreaF.right(), int( boxAreaF.width() ) + 1, int( boxAreaF.height() ) + 1 );
                    QRect boxArea( boxAreaF.left(), boxAreaF.top(), 1, 1 );
                    boxArea.setRight( int(boxAreaF.right()) );
                    boxArea.setBottom( int(boxAreaF.bottom()) );

                    boxArea = boxArea.intersected( inputArea );

                    if( !boxArea.isEmpty() )
                    {
                        // Sum of all pixel values
                        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
                        {
                            float ratio = 1 - (boxAreaF.top() - j);
                            if( ratio < 1.0 )
                            {
                                yRatio = ratio;
                            }
                            else
                            {
                                yRatio = std::min( 1.F - (float(j+1) - float(boxAreaF.bottom())), 1.F);
                            }

                            sourceScanline = inputData + j * sourceBPL + boxArea.left() * 4;
                            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
                            {
                                ratio = 1.F - (boxAreaF.left() - i);
                                if( ratio < 1.0 )
                                {
                                    xRatio = ratio;
                                }
                                else
                                {
                                    xRatio = std::min( 1.F - (float(i+1) - float(boxAreaF.right())), 1.F );
                                }

                                finalRatio = xRatio * yRatio;

                                bSum += *sourceScanline * finalRatio; ++sourceScanline;
                                gSum += *sourceScanline * finalRatio; ++sourceScanline;
                                rSum += *sourceScanline * finalRatio; ++sourceScanline;
                                aSum += *sourceScanline * finalRatio; ++sourceScanline;

                                surface += finalRatio;
                            }
                        }

                        surface = floatBoxWidth * floatBoxHeight;
                        rSum /= surface;
                        gSum /= surface;
                        bSum /= surface;
                        aSum /= surface;


                        const float inverseCeiled = 1 - (aSum / maxAlphaRanged);

                        float* gPtr = stampScan + 1;
                        float* rPtr = stampScan + 2;
                        float* alphaPtr = stampScan + 3;

                        *stampScan  = bSum + *stampScan  * inverseCeiled;
                        *gPtr       = gSum + *gPtr       * inverseCeiled;
                        *rPtr       = rSum + *rPtr       * inverseCeiled;
                        *alphaPtr   = aSum + *alphaPtr   * inverseCeiled;

                        const float transparencyAmountInverse = 1 - *alphaPtr / 255.F;

                        *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;
                        ++stampScan;

                        *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;
                        ++stampScan;

                        *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;
                        ++stampScan;

                        *destScanline = *stampScan + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;
                        ++stampScan;


                        // Blend
                        //BlendPixelNormalFParrallel( &destScanline, &parallelScanline, rSum, gSum, bSum, aSum );

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

