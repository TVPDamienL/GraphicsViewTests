#pragma once

#include "Image.Utilities.h"
#include "Thread.h"
#include "ThreadMachine.h"

#include <QRect>
#include <QTransform>
#include <QPolygon>

#include "ImageDebugger.h"



#include "Debug.h"


static
inline
void
ReadGrayImageWithFloatArea( int iX, int iY, float iTopLeftRatio, float iTopRightRatio, float iBottomLeftRatio, float iBottomRightRatio,
                        const float* iImage, int iImageWidth, int iImageHeight,
                        float* oAlpha )
{
    const float* scanline;
    const int bpl = iImageWidth;

    int minX = iX - 1;
    int minY = iY - 1;

    const int maxX = minX + 1;
    const int maxY = minY + 1;

    *oAlpha = 0.F;

    scanline = iImage + minY * bpl + minX;
    // Top left
    if( minX >= 0 && minX < iImageWidth && minY >= 0 && minY < iImageHeight )
        *oAlpha += *scanline * iTopLeftRatio;

    ++scanline;

    // Top right
    if( maxX >= 0 && maxX < iImageWidth && minY >= 0 && minY < iImageHeight )
        *oAlpha += *scanline * iTopRightRatio;

    ++scanline;

    scanline += bpl - 2;
    // Bottom left
    if( minX >= 0 && minX < iImageWidth && maxY >= 0 && maxY < iImageHeight )
        *oAlpha += *scanline * iBottomLeftRatio;

    ++scanline;

    // Bottom Right
    if( maxX >= 0 && maxX < iImageWidth && maxY >= 0 && maxY < iImageHeight )
        *oAlpha += *scanline * iBottomRightRatio;

    ++scanline;
}


static
inline
void
ReadBGRAImageWithFloatArea( int iX, int iY, float iTopLeftRatio, float iTopRightRatio, float iBottomLeftRatio, float iBottomRightRatio,
                            const float* iImage, int iWidth, int iHeight,
                            float* oRed, float* oGreen, float* oBlue, float* oAlpha )
{
    const float* scanline;
    const int bpl = iWidth * 4;

    int minX = iX - 1;
    int minY = iY - 1;

    const int maxX = minX + 1;
    const int maxY = minY + 1;

    const int width = iWidth;
    const int height = iHeight;

    *oRed   = 0.F;
    *oGreen = 0.F;
    *oBlue  = 0.F;
    *oAlpha = 0.F;

    scanline = iImage + minY * bpl + minX * 4;
    // Top left
    if( minX >= 0 && minX < width && minY >= 0 && minY < height )
    {
        *oBlue  += *scanline * iTopLeftRatio;++scanline;
        *oGreen += *scanline * iTopLeftRatio;++scanline;
        *oRed   += *scanline * iTopLeftRatio;++scanline;
        *oAlpha += *scanline * iTopLeftRatio;++scanline;
    }
    else
    {
        scanline += 4;
    }

    // Top right
    if( maxX >= 0 && maxX < width && minY >= 0 && minY < height )
    {
        *oBlue  += *scanline * iTopRightRatio;++scanline;
        *oGreen += *scanline * iTopRightRatio;++scanline;
        *oRed   += *scanline * iTopRightRatio;++scanline;
        *oAlpha += *scanline * iTopRightRatio;++scanline;
    }
    else
    {
        scanline += 4;
    }

    scanline += bpl - 8;
    // Bottom left
    if( minX >= 0 && minX < width && maxY >= 0 && maxY < height )
    {
        *oBlue  += *scanline * iBottomLeftRatio;++scanline;
        *oGreen += *scanline * iBottomLeftRatio;++scanline;
        *oRed   += *scanline * iBottomLeftRatio;++scanline;
        *oAlpha += *scanline * iBottomLeftRatio;++scanline;
    }
    else
    {
        scanline += 4;
    }

    // Bottom Right
    if( maxX >= 0 && maxX < width && maxY >= 0 && maxY < height )
    {
        *oBlue  += *scanline * iBottomRightRatio;++scanline;
        *oGreen += *scanline * iBottomRightRatio;++scanline;
        *oRed   += *scanline * iBottomRightRatio;++scanline;
        *oAlpha += *scanline * iBottomRightRatio;++scanline;
    }
}


static
void
MTBlendImageNormalFSubpixel(    const float* iSource, const int iSourceWidth, const int iSourceHeight, const QRectF& iSourceArea,
                                float* iDestination, const int iDestinationWidth, const int iDestinationHeight, const QPoint& iPoint,
                                const float iOpacity )
{
    int sourceBPL = iSourceWidth*4;
    int dstBPL = iDestinationWidth*4;
    //int alphaBPL = alphaW;

    const QPointF srcDstOffset = iPoint - iSourceArea.topLeft();
    //const QPoint offsetAlpha = alphaPoint - iSourceArea.topLeft();

    QRectF srcArea( 0, 0, iSourceWidth, iSourceHeight );
    QRectF dstArea( 0, 0, iDestinationWidth, iDestinationHeight );
    //QRect alphaArea( 0, 0, alphaW, alphaH );

    // Clip to src rect
    QRectF workingArea = srcArea.intersected( iSourceArea );

    // Clip to dst rect
    workingArea = workingArea.translated( srcDstOffset );
    workingArea = workingArea.intersected( dstArea );
    workingArea = workingArea.translated( -srcDstOffset );

    // Clip to alpha rect
    //workingArea = workingArea.translated( offsetAlpha );
    //workingArea = workingArea.intersected( alphaArea );
    //workingArea = workingArea.translated( -offsetAlpha );

    const QPointF point = iSourceArea.topLeft();
    const QPoint intPoint( point.x(), point.y() );
    const QPointF subReadOffset = - (point - intPoint); // Basically the offset representing the amount that has been cut by int

                                                        // Keep offset between 0 and 1
                                                        // TODO: if offset is larger than 1 or -1, need to do multiple +1 or -1 : 2.16 -> 0.16
                                                        // noneed: indeed sudReadBuffer is the subtraction between a flot and its int cut, which can't be > 1
    QPointF offsetFinal = subReadOffset;
    offsetFinal.setX( offsetFinal.x() + 1 );
    offsetFinal.setY( offsetFinal.y() + 1 );

    float topLeftRatio      = (1-offsetFinal.y())   * (1-offsetFinal.x());
    float topRightRatio     = (1-offsetFinal.y())   * offsetFinal.x();
    float bottomRightRatio  = offsetFinal.y()       * offsetFinal.x();
    float bottomLeftRatio   = offsetFinal.y()       * (1-offsetFinal.x());



    const int startingX   = workingArea.left();
    const int endingX     = workingArea.right() + 1;
    const int startingY   = workingArea.top();
    const int endingY     = workingArea.bottom() + 1;

    int height = endingY - startingY;

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
            const float* sourceScanline = iSource;
            float* destScanline = iDestination;
            //const float* alphaScan = alphaMask;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;

            for( int y = startY; y < endY; ++y )
            {
                sourceScanline  = iSource + y * sourceBPL + startX * 4;
                //destScanline    = iDestination + (y + int(srcDstOffset.y())) * dstBPL + (startX + int(srcDstOffset.x())) * 4;
                //destScanline    = iDestination + (y - startingY) * dstBPL;
                destScanline    = iDestination  + (y - startingY) * dstBPL;

                //alphaScan    = alphaMask + (y + offsetAlpha.y()) * alphaBPL + (startX + offsetAlpha.x());

                for( int x = startX; x < endX; ++x )
                {
                    if( (y - startingY) >= iDestinationHeight || (x - startingX) >= iDestinationWidth )
                        qDebug() << "Merde";

                    float red, green, blue, alp;
                    ReadBGRAImageWithFloatArea( x, y, topLeftRatio, topRightRatio, bottomLeftRatio, bottomRightRatio,
                                                iSource, iSourceWidth, iSourceHeight,
                                                &red, &green, &blue, &alp );

                    if( alp == 0 ) // Skip if alpha is nil
                    {
                        sourceScanline += 4;
                        destScanline += 4;
                        //++alphaScan;
                        continue;
                    }

                    // This would allow to pick color using the tip shape, but it leaves a lot of mistakes, and i dunno if it's subpixel related atm
                    float v = /**alphaScan / 255.F **/ iOpacity;

                    BlendPixelNormalF( &destScanline, red * v, green * v, blue * v, alp * v );
                    sourceScanline += 4;
                    //++alphaScan;
                }
            }
        },
            cRange( startingX, startingY + i * split ), cRange( endingX - startingX, split + correct ), true ) );
        // +1 because we range over height amount
        // It then works with endX being startY + iRange.mY;
        // meaning if range is 25 ( we go 0-24 ), endY will be 25, so we THEN do < in the for
    }
}


static
void
MTBlendImageNormalFDry( const float* source, const int iSourceWidth, const int iSourceHeight,  // Source we will blend, in Gray
                        const float* iColorBuffer, const int iColorW, const int iColorH,   // The color stamp buffer, to lookup color to apply

                        const float* drybuffer, const int iWidth, const int iHeight,      // The background over which we blend
                        float* stampbuffer,

                        float* destination,         // The float output result buffer
                        QImage* iParallelRender,    // The 8bit output buffer
                        const QImage* iAlphaMask,
                        const QPointF& point,
                        const float maxAlpha,
                        bool iDryActive )
{
    //float* tmp = new float[ (iSourceWidth+1)*4 * (iSourceHeight+1) ];


    const uchar maxAlphaRanged = maxAlpha * 255.F;
    const int sourceBPL = iSourceWidth;
    const int colorBPL = iColorW*4;
    const int buffersBPL = iWidth * 4;

    uchar* parallelData = iParallelRender->bits();
    const int parallelBPL = iParallelRender->bytesPerLine();

    const uchar* alphaData = iAlphaMask->bits();
    const int    alphaBPL  = iAlphaMask->bytesPerLine();

    // Final area in float
    QRectF floatArea( point.x(), point.y(), iSourceWidth, iSourceHeight );

    // Because int cuts will cut lower value if > 0 :  1.26 ->  1
    //                             and upper if < 0 : -1.26 -> -1
    QPoint intPoint( point.x(), point.y() );
    if( point.x() < 0 )
        intPoint.setX( intPoint.x() - 1 );
    if( point.y() < 0 )
        intPoint.setY( intPoint.y() - 1 );

    const QPointF subReadOffset = - (point - intPoint); // Basically the offset representing the amount that has been cut by int

    // Keep offset between 0 and 1
    // TODO: if offset is larger than 1 or -1, need to do multiple +1 or -1 : 2.16 -> 0.16
    // noneed: indeed sudReadBuffer is the subtraction between a flot and its int cut, which can't be > 1
    QPointF offsetFinal = subReadOffset;
    offsetFinal.setX( offsetFinal.x() + 1 );
    offsetFinal.setY( offsetFinal.y() + 1 );
    //offsetFinal.setX( fmodf( offsetFinal.x() + 1, 1 ) );
    //offsetFinal.setY( fmodf( offsetFinal.y() + 1, 1 ) );

    float topLeftRatio      = (1-offsetFinal.y())   * (1-offsetFinal.x());
    float topRightRatio     = (1-offsetFinal.y())   * offsetFinal.x();
    float bottomRightRatio  = offsetFinal.y()       * offsetFinal.x();
    float bottomLeftRatio   = offsetFinal.y()       * (1-offsetFinal.x());

    // Source bbox
    const int minX = point.x();
    const int maxX = minX + iSourceWidth; // +1 Because we want to go over one more pixel, as float will reach things like 1.2, need to not stop at pixel 1, but 2
    const int minY = point.y();
    const int maxY = minY + iSourceHeight;

    // Clipped bounds == iteration limits
    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= iWidth ? iWidth : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= iHeight ? iHeight : maxY;
    const int height = endingY - startingY;

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
            const float* colorScan = iColorBuffer;
            const float* dryScan = drybuffer;
            float* stampScan = stampbuffer;
            float* destScanline = destination;
            uchar* parallelScanline = parallelData;
            const uchar* alphaScanline = alphaData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY;
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX;
            const int xOffset = startX * 4;

            float yRatio = 1.F;
            float xRatio = 1.F;

            for( int y = startY; y < endY; ++y )
            {
                // Indexes and stuff
                colorScan  = iColorBuffer + (y - minY) * colorBPL + (startX - minX) * 4;

                // All these are the same size
                // This one is gray
                stampScan = stampbuffer + y * iWidth + startX;

                // Theses are BGRA, float or uint8
                const int indexOffset = y * buffersBPL + xOffset;
                dryScan             = drybuffer + indexOffset;                  // float
                destScanline        = destination + indexOffset;                // float

                if( iDryActive )
                    dryScan = destScanline;                                                         // Switch to not using dry A

                parallelScanline    = parallelData + y * parallelBPL + xOffset; // uint8
                alphaScanline       = alphaData + y * alphaBPL + xOffset + 3;   // float

                for( int x = startX; x < endX; ++x )
                {
                    float alpha = 0;
                    ReadGrayImageWithFloatArea( (x - minX), (y - minY), topLeftRatio, topRightRatio, bottomLeftRatio, bottomRightRatio,
                                            source, iSourceWidth, iSourceHeight,
                                            &alpha );

                    //tmp[ (y - minY) * (iSourceWidth+1) + (x - minX) ] = alpha;
                    if( alpha == 0 ) // Skip if alpha is nil
                    {
                        stampScan++;
                        colorScan += 4;
                        dryScan += 4;
                        destScanline += 4;

                        parallelScanline += 4;
                        alphaScanline += 4;
                        continue;
                    }


                    // Alpha masking
                    const float alphaMaskMult = float(*alphaScanline) / 255.F;
                    alphaScanline += 4;
                    alpha *= alphaMaskMult;

                    // Blending in stamp buffer
                    const float inverseCeiled = (1 - (alpha / maxAlphaRanged));

                    if( iDryActive )
                        *stampScan  = alpha;
                    else
                        *stampScan  = alpha + *stampScan  * inverseCeiled;                           // Switch to not using dry B

                    // Blending stampbuffer over drybuffer in floatbuffer and parallelRender at the same time
                    const float stampAlphaNorm = *stampScan / 255.F;

                    // Inverse of the alpha of the combination of color and stamp
                    const float transparencyAmountInverse = 1 - *(colorScan+3)/255.F * stampAlphaNorm;

                    // Then we blend final color, being color * stamp alpha, into the dry buffer
                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++colorScan;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++colorScan;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++colorScan;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++destScanline;
                    ++dryScan;
                    ++colorScan;
                    ++parallelScanline;
                    ++stampScan;

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

    //IMAGEDEBUG->ShowImage( tmp, (iSourceWidth+1), (iSourceHeight+1) );
    //qDebug() << "Off : " << -subReadOffset;
}



static
void
ReadSubPixelGreyF( float* oSum, const QPointF& iPoint, const float iBoxWidth, const float iBoxHeight, const float* srcImage, const int sourceBPL, bool iTruncate, const QRect& iTruncateArea )
{
    // Get the box to read in original
    QRectF boxAreaF( iPoint.x(), iPoint.y(), iBoxWidth, iBoxHeight );
    QRect boxArea( boxAreaF.left(), boxAreaF.top(), 1, 1 );
    boxArea.setRight( int(boxAreaF.right()) );
    boxArea.setBottom( int(boxAreaF.bottom()) );

    // This is the security line, that prevents from reading out of source buffer
    // Because we divide by the total surface at the end, skipping (because of interect clamp) pixels will reduce overall intensity properly
    // ( they will count as 0, which is correct because out of buffer = transparency )
    if( iTruncate )
        boxArea = boxArea.intersected( iTruncateArea );

    *oSum = 0;
    const float* dataOffset = srcImage + boxArea.left();
    const float surface = iBoxWidth * iBoxHeight;
    float xRatio = 1.0;
    float yRatio = 1.0;
    float finalRatio = 1.0;

    if( !boxArea.isEmpty() )
    {
        // Sum of all pixel values
        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
        {
            yRatio = 1 - (boxAreaF.top() - j);
            if( yRatio > 1.0 )
            {
                yRatio = std::min( 1.F - (float(j+1) - float(boxAreaF.bottom())), 1.F);
            }

            const float* sourceScanline = dataOffset + j * sourceBPL;
            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
            {
                xRatio = 1.F - (boxAreaF.left() - i);

                if( xRatio > 1.0 )
                {
                    xRatio = std::min( 1.F - (float(i+1) - float(boxAreaF.right())), 1.F );
                }

                finalRatio = xRatio * yRatio;

                *oSum += *sourceScanline * finalRatio; ++sourceScanline;
            }
        }

        *oSum /= surface;
    }
}



static
void
TrusssscRGBA( float* oRed, float* oGreen, float* oBlue, float* oAlpha, const QPointF& iPoint, const float iBoxWidth, const float iBoxHeight, const float* srcImage, const int sourceBPL, bool iTruncate, const QRect& iTruncateArea )
{
    // Get the box to read in original
    QRectF boxAreaF( iPoint.x(), iPoint.y(), iBoxWidth, iBoxHeight );
    QRect boxArea( boxAreaF.left(), boxAreaF.top(), 1, 1 );
    boxArea.setRight( int(boxAreaF.right()) );
    boxArea.setBottom( int(boxAreaF.bottom()) );

    // This is the security line, that prevents from reading out of source buffer
    // Because we divide by the total surface at the end, skipping (because of interect clamp) pixels will reduce overall intensity properly
    // ( they will count as 0, which is correct because out of buffer = transparency )
    if( iTruncate )
        boxArea = boxArea.intersected( iTruncateArea );

    *oRed = 0;
    *oGreen = 0;
    *oBlue = 0;
    *oAlpha = 0;

    const float* dataOffset = srcImage + boxArea.left() * 4;
    const float surface = iBoxWidth * iBoxHeight;
    float xRatio = 1.0;
    float yRatio = 1.0;
    float finalRatio = 1.0;

    if( !boxArea.isEmpty() )
    {
        // Sum of all pixel values
        for( int j = boxArea.top(); j <= boxArea.bottom(); ++j )
        {
            yRatio = 1 - (boxAreaF.top() - j);
            if( yRatio > 1.0 )
            {
                yRatio = std::min( 1.F - (float(j+1) - float(boxAreaF.bottom())), 1.F);
            }

            const float* sourceScanline = dataOffset + j * sourceBPL;
            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
            {
                xRatio = 1.F - (boxAreaF.left() - i);

                if( xRatio > 1.0 )
                {
                    xRatio = std::min( 1.F - (float(i+1) - float(boxAreaF.right())), 1.F );
                }

                finalRatio = xRatio * yRatio;

                *oBlue  += *sourceScanline * finalRatio; ++sourceScanline;
                *oGreen += *sourceScanline * finalRatio; ++sourceScanline;
                *oRed   += *sourceScanline * finalRatio; ++sourceScanline;
                *oAlpha += *sourceScanline * finalRatio; ++sourceScanline;
            }
        }

        *oRed /= surface;
        *oGreen /= surface;
        *oBlue /= surface;
        *oAlpha /= surface;
    }
}



// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
static
void
MTDownscaleBoxAverageDirectAlphaFDry( const float* iInput, const int iWidth, const int iHeight,         // Source we will blend => The mipmap subimage
                                      const float* iColorBuffer, const int iColorW, const int iColorH,   // The color stamp buffer, to lookup color to apply

                                      bool iSkipColorSubSampling,
                                                                        // All float images MUST be the same size, this allows faster access
                                    const float* background, const int iOutputBuffersWidth, const int iOutputBuffersHeight,    // The background over which we blend
                                    float* stampBuffer,                                              // The buffer holding the current tool line

                                    float* iOutput,                                                  // The output result buffer, can be the same as background
                                    QImage* iParallelRender,                                             // The uchar buffer we wanna write to at the same time
                                    const QImage* iAlphaMask,
                                    const QTransform& iTransform, const QPoint& iOrigin, // Transform and its origin to apply
                                    const float maxalpha,
                                    bool iDryActive,
                                    const QPointF& colorOffset )
{
    const int inputWidth = iWidth;
    const int inputHeight = iHeight;
    QRect inputArea = QRect( 0, 0, iWidth, iHeight );
    inputArea.moveTopLeft( iOrigin );
    QRectF inputAreaF = QRect( 0, 0, iWidth, iHeight );

    // Transformed bbox
    QPolygonF outputRect = MapToPolygonF( iTransform, inputArea );

    QRect transfoBBoxI = ExclusiveBoundingBox( outputRect );
    QRectF transfoBBoxF = ExclusiveBoundingBoxF( outputRect );
    const QPointF subPixelPos = transfoBBoxF.topLeft();

    // Add one because float will go from 0.2 to 1.2 for example
    // So we need to cover pixels from 0 to 2
    // 0 is the min being int cut, 2 is max being int cut + 1
    transfoBBoxI.setWidth( transfoBBoxI.width() + 1 );
    transfoBBoxI.setHeight( transfoBBoxI.height() + 1 );

    int minX = transfoBBoxI.left();
    int minY = transfoBBoxI.top();

    transfoBBoxI = transfoBBoxI.intersected( QRect( 0, 0, iOutputBuffersWidth, iOutputBuffersHeight ) ); // Intersected here will be inclusive, so intersection with 1080 and 1086 = 1080 for x2

    // Scales
    const float transfoWidth = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] );
    const float transfoHeight = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] );

    const double xScaleFactorOriginal = transfoWidth /      inputAreaF.width() ;
    const double yScaleFactorOriginal = transfoHeight /     inputAreaF.height();
    const double xScaleFactorRounded = int(transfoWidth) /  inputAreaF.width() ;
    const double yScaleFactorRounded = int(transfoHeight) / inputAreaF.height();

    if( xScaleFactorOriginal >= 1.0 && yScaleFactorOriginal >= 1.0 )
    {
        MTBlendImageNormalFDry( iInput, iWidth, iHeight,
                                iColorBuffer, iColorW, iColorH,
                                background, iOutputBuffersWidth, iOutputBuffersHeight,
                                stampBuffer, iOutput,
                                iParallelRender,
                                iAlphaMask,
                                subPixelPos,
                                maxalpha,
                                iDryActive );
        return;
    }


    //qDebug() << "========================================================================";
    //qDebug() << "========================================================================";
    //qDebug() << "========================================================================";
    //qDebug() << "========================================================================";


    QTransform inverse = iTransform.inverted();
    const uchar maxAlphaRanged = maxalpha * 255.F;

    const float floatBoxWidth = inputAreaF.width() / transfoWidth;
    const float floatBoxHeight = inputAreaF.height() / transfoHeight;

    // Data iteration
    uchar* parallelData     = iParallelRender->bits();

    const int sourceBPL     = iWidth; // not *4 because gray, not bgra
    const int colorBPL      = iColorW * 4;
    const int buffersBPL    = iOutputBuffersWidth * 4;
    const int parallelBPL   = iParallelRender->bytesPerLine();

    const uchar* alphaData  = iAlphaMask->bits();
    const int alphaBPL      = iAlphaMask->bytesPerLine();

    const int startingX = transfoBBoxI.left();
    const int startingY = transfoBBoxI.top();
    const int endingX   = transfoBBoxI.right() >= iOutputBuffersWidth ? iOutputBuffersWidth - 1 : transfoBBoxI.right();
    const int endingY   = transfoBBoxI.bottom() >= iOutputBuffersHeight ? iOutputBuffersHeight - 1 : transfoBBoxI.bottom();
    const int height    = endingY - startingY + 1;


    // We offset to the mipmap's full image, so we then apply the same ratios used in downsampling to read the colorstamp
    const float xColorOffset = (iColorW - transfoBBoxF.width())  / 2;
    const float yColorOffset = (iColorH - transfoBBoxF.height()) / 2;

    const float finalMarginX = transfoBBoxI.right() - transfoBBoxF.right();
    const float finalMarginY = transfoBBoxI.bottom() - transfoBBoxF.bottom();


    //const int threadCount = 1;
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
            const float* sourceScanline = iInput;
            const float* colorScan = iColorBuffer;
            const uchar* alphaScanline = alphaData + 3;
            float* stampScan = stampBuffer;

            const float*    dryScan         = background;
            float*          destScanline    = iOutput;

            uchar* parallelScanline = parallelData;

            const int startY = iOff.mY;
            const int endY = startY + iRange.mY - 1; // -1 because offset is 0 based, and range is how many time you do stuff
                                                     // So, range 25 = do 25 times ( 0 to 24 --> range - 1 )
            const int startX = iOff.mX;
            const int endX = startX + iRange.mX - 1;
            const int xOffset = startX * 4;

            // Pixel averaging variables
            float stampAlphaSum = 0;
            float colorAlpha  = iColorBuffer[ 3 ];

            for( int y = startY; y <= endY; ++y )
            {
                // Gray buffers
                sourceScanline  = iInput + (y - minY) * sourceBPL + (startX - minX);    // Gray
                stampScan       = stampBuffer +  y * iOutputBuffersWidth + startX;      // Gray

                // Float buffers
                colorScan = iColorBuffer + ((y-startingY) + int(colorOffset.y())) * colorBPL + int(colorOffset.x()) * 4;
                const int indexOffset = y * buffersBPL + xOffset;
                dryScan  = background + indexOffset;
                destScanline   = iOutput + indexOffset;
                if( iDryActive )
                    dryScan    = destScanline;

                // uchar datas
                parallelScanline    = parallelData + y * parallelBPL + xOffset;
                alphaScanline = alphaData + y * alphaBPL + xOffset + 3;

                for( int x = startX; x <= endX; ++x )
                {
                    // Reads the tip with subpixel, so the output tip shape is subpixeled
                    // The color isn't subpixeled so it doesn't shift from one sample to the next
                    ReadSubPixelGreyF( &stampAlphaSum, inverse.map( QPointF( x, y ) ), floatBoxWidth, floatBoxHeight, iInput, sourceBPL, x == startingX || x == endingX || y == startingY || y == endingY, inputArea );

                    // Here alpha is used as opacity mask
                    // If we want to use alpha as a light map, do the same but multiply the amount of light by this 0-1 value etc...
                    // This would prolly require a new alpha image though, as this alpha is the one used for selection mask etc..., so it can't be replaced
                    const float alphaMaskMult = *alphaScanline / 255.F;
                    stampAlphaSum *= alphaMaskMult;
                    alphaScanline += 4;

                    // Drawing in stamp buffer
                    const float inverseCeiled = 1 - (stampAlphaSum / maxAlphaRanged);

                    if( iDryActive )
                        *stampScan   = stampAlphaSum;
                    else
                        *stampScan   = stampAlphaSum + *stampScan   * inverseCeiled;

                    // Then drawing the final stamp to the float output and 8bit final image at the same time
                    const float stampAlphaNorm = *stampScan / 255.F;

                    // Inverse of the alpha of the combination of color and stamp
                    //const float transparencyAmountInverse = 1 - *(colorScan+3)/255.F * stampAlphaNorm;
                    const float transparencyAmountInverse = 1 - colorAlpha/255.F * stampAlphaNorm;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++colorScan;
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++colorScan;
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++colorScan;
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;

                    *destScanline = *colorScan * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                    *parallelScanline = uchar( *destScanline );
                    ++colorScan;
                    ++destScanline;
                    ++dryScan;
                    ++parallelScanline;
                    ++stampScan;

                    stampAlphaSum = 0;
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























//======================================================
//======================================================
//======================================================
//======================================================
// UNUSED
//======================================================
//======================================================
//======================================================
//======================================================





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

