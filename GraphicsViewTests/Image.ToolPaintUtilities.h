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
ReadGrayImageWithFloatArea( const QPointF& iOffset, int iX, int iY, float iTopLeftRatio, float iTopRightRatio, float iBottomLeftRatio, float iBottomRightRatio,
                        const float* iImage, int iImageWidth, int iImageHeight,
                        float* oAlpha )
{
    const float* scanline;
    const int bpl = iImageWidth;

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
ReadBGRAImageWithFloatArea( const QPointF& iOffset, int iX, int iY, float iTopLeftRatio, float iTopRightRatio, float iBottomLeftRatio, float iBottomRightRatio,
                            const float* iImage, int iWidth, int iHeight,
                            float* oRed, float* oGreen, float* oBlue, float* oAlpha )
{
    const float* scanline;
    const int bpl = iWidth * 4;

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
    const int endingX = maxX >= iWidth ? iWidth : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= iHeight ? iHeight : maxY;
    const int height = endingY - startingY; // No +1 because we are using straight value, not a .right from QRect, that does a width() -1

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
                    ReadGrayImageWithFloatArea( subReadOffset, (x - minX), (y - minY), topLeftRatio, topRightRatio, bottomLeftRatio, bottomRightRatio,
                                            source, iSourceWidth, iSourceHeight,
                                            &alpha );

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
}





// iTransform should be a downscale, otherwise it's not ment to work
// This averages pixels to get the condensed pixel
// PERFORMANCE : Not a huge increase from doing a new allocated image output that we blend afterward and this
// where we directly draw to output
static
void
MTDownscaleBoxAverageDirectAlphaFDry( const float* iInput, const int iWidth, const int iHeight,         // Source we will blend => The mipmap subimage
                                      const float* iColorBuffer, const int iColorW, const int iColorH,   // The color stamp buffer, to lookup color to apply

                                      bool iIsColorUniform,
                                                                        // All float images MUST be the same size, this allows faster access
                                    const float* background, const int iOutputBuffersWidth, const int iOutputBuffersHeight,    // The background over which we blend
                                    float* stampBuffer,                                              // The buffer holding the current tool line

                                    float* iOutput,                                                  // The output result buffer, can be the same as background
                                    QImage* iParallelRender,                                             // The uchar buffer we wanna write to at the same time
                                    const QImage* iAlphaMask,
                                    const QTransform& iTransform, const QPoint& iOrigin, // Transform and its origin to apply
                                    const float maxalpha,
                                    bool iDryActive )
{
    const int inputWidth = iWidth;
    const int inputHeight = iHeight;
    QRect inputArea = QRect( 0, 0, iWidth, iHeight );
    inputArea.moveTopLeft( iOrigin );
    QRectF inputAreaF = QRect( 0, 0, iWidth, iHeight );

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

    // Scales
    const float transfoWidth = Distance2Points( outputRect[ 0 ], outputRect[ 1 ] );
    const float transfoHeight = Distance2Points( outputRect[ 1 ], outputRect[ 2 ] );

    const double xScaleFactorOriginal = transfoWidth /      inputAreaF.width() ;
    const double yScaleFactorOriginal = transfoHeight /     inputAreaF.height();
    const double xScaleFactorRounded = int(transfoWidth) /  inputAreaF.width() ;
    const double yScaleFactorRounded = int(transfoHeight) / inputAreaF.height();

    if( xScaleFactorOriginal >= 1.0 || yScaleFactorOriginal >= 1.0 )
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


    // We decide to work with ratios resulting from the rounded result
    // Any scale would provide a floating output size, let's say 4.48 for example
    // Final result will be 4 pixels wide, not 4.48, as we need int values.
    // Now, dividing the image in 4.48 will result in missing datas (because for loop below will go 4 iterations, not 4.48,, and 4*X < 4.48*X),
    // and thus, uneven results at lower sizes
    // So we take the final int size, compute the scale this represents, and set this one as the transformation scale, so we can evenly cover the original image
    QTransform inverse = iTransform.inverted();

    const float floatBoxWidth = inputAreaF.width() / transfoWidth;
    const float floatBoxHeight = inputAreaF.height() / transfoHeight;

    //float xBaseRatio      = 1+subpixelOffset.x(); // Because subpixelOffset is always < 0


    // Data iteration
    const float * inputData = iInput;
    float* outputData       = iOutput;
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


    // Because int cuts will cut lower value if > 0 :  1.26 ->  1
    //                             and upper if < 0 : -1.26 -> -1
    QPoint intPoint = transfoBBoxI.topLeft();   // The very final box' top left int rounded
    QPointF point = transfoBBoxF.topLeft();     // The very final box' top left
    if( point.x() < 0 )
        intPoint.setX( intPoint.x() - 1 );
    if( point.y() < 0 )
        intPoint.setY( intPoint.y() - 1 );

    const QPointF subReadOffset = - (point - intPoint); // Basically the offset representing the amount that has been cut by int

    QPointF offsetFinal = subReadOffset;
    offsetFinal.setX( offsetFinal.x() + 1 );
    offsetFinal.setY( offsetFinal.y() + 1 );

    float topLeftRatio      = (1-offsetFinal.y())   * (1-offsetFinal.x());
    float topRightRatio     = (1-offsetFinal.y())   * offsetFinal.x();
    float bottomRightRatio  = offsetFinal.y()       * offsetFinal.x();
    float bottomLeftRatio   = offsetFinal.y()       * (1-offsetFinal.x());


    // We offset to the mipmap's full image, so we then apply the same ratios used in downsampling to read the colorstamp
    const int xColorOffset = (iColorW - transfoBBoxI.width())  / 2  +1; // +1 seems to center properly the colorBox, but i'm not sure why .. Otherwise it shifts bottom right ( == we read too much topLeft )
    const int yColorOffset = (iColorH - transfoBBoxI.height()) / 2  +1;


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

            float redSum = iColorBuffer[ 2 ];
            float greenSum = iColorBuffer[ 1 ];
            float blueSum = iColorBuffer[ 0 ];
            float alphaSum = iColorBuffer[ 3 ];

            const float surface = floatBoxWidth * floatBoxHeight;
            float xRatio = 1.0;
            float yRatio = 1.0;
            float finalRatio = 1.0;

            for( int y = startY; y <= endY; ++y )
            {
                // Gray buffers
                sourceScanline  = iInput + (y - minY) * sourceBPL + (startX - minX);                                        // Gray
                stampScan       = stampBuffer +  y * iOutputBuffersWidth + startX;                                          // Gray

                // Float buffers
                const int indexOffset = y * buffersBPL + xOffset;
                dryScan  = background + indexOffset;
                destScanline    = iOutput + indexOffset;
                if( iDryActive )
                    dryScan    = destScanline;

                // uchar datas
                parallelScanline    = parallelData + y * parallelBPL + xOffset;
                alphaScanline = alphaData + y * alphaBPL + xOffset + 3;

                for( int x = startX; x <= endX; ++x )
                {
                    // Get the point in original
                    const QPointF xyMappedF = inverse.map( QPointF( x, y ) ); // To use this wee need to tune the transformation with new scale
                    const QPoint xyMapped = QPoint( xyMappedF.x(), xyMappedF.y() );

                    // Get the box to read in original
                    QRectF boxAreaF( xyMappedF.x(), xyMappedF.y(), floatBoxWidth, floatBoxHeight );
                    QRect boxArea( boxAreaF.left(), boxAreaF.top(), 1, 1 );
                    boxArea.setRight( int(boxAreaF.right()) );
                    boxArea.setBottom( int(boxAreaF.bottom()) );
                    // This is the security line, that prevents from reading out of source buffer
                    // Because we divide by the total surface at the end, skipping (because of interect clamp) pixels will reduce overall intensity properly
                    // ( they will count as 0, which is correct because out of buffer = transparency )
                    if( x == startingX || x == endingX || y == startingY || y == endingY )
                        boxArea = boxArea.intersected( inputArea );

                    const float* dataOffset = inputData + boxArea.left();

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

                            sourceScanline = dataOffset + j * sourceBPL;

                            for( int i = boxArea.left(); i <= boxArea.right(); ++i )
                            {
                                xRatio = 1.F - (boxAreaF.left() - i);

                                if( xRatio > 1.0 )
                                {
                                    xRatio = std::min( 1.F - (float(i+1) - float(boxAreaF.right())), 1.F );
                                }

                                finalRatio = xRatio * yRatio;

                                stampAlphaSum   += *sourceScanline * finalRatio; ++sourceScanline;
                            }
                        }

                        if( !iIsColorUniform )
                        {
                            ReadBGRAImageWithFloatArea( subReadOffset, (x - minX + xColorOffset), (y - minY + yColorOffset), topLeftRatio, topRightRatio, bottomLeftRatio, bottomRightRatio,
                                                        iColorBuffer, iColorW, iColorH,
                                                        &redSum, &greenSum, &blueSum, &alphaSum );
                        }

                        stampAlphaSum   /= surface;

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
                        const float transparencyAmountInverse = 1 - alphaSum/255.F * stampAlphaNorm;

                        *destScanline = blueSum * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;

                        *destScanline = greenSum * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;

                        *destScanline = redSum * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;

                        *destScanline = alphaSum * stampAlphaNorm + *dryScan * transparencyAmountInverse;
                        *parallelScanline = uchar( *destScanline );
                        ++destScanline;
                        ++dryScan;
                        ++parallelScanline;
                        ++stampScan;

                        stampAlphaSum = 0;
                    }
                    else
                    {
                        // Do all the ++ and += 4 ?
                        int bp  =5 ;
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

