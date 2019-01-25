#include "cConvolution.h"


cConvolution::~cConvolution()
{
}


cConvolution::cConvolution()
{
}


void
cConvolution::SetKernel( const QGenericMatrix< 3, 3, int > & iKernel )
{
    mKernel = iKernel;
}


QImage*
cConvolution::GetEdgeDectionFromImage( const QImage* iImage )
{
    // Doesn't need mKernel
    // Kernel used are predefined

    // This convolution doesn't do a division by sum of kernel values, as they are 0
    // It actually doesn't apply anything, it's just to catch pixel variation
    // Then, if any variation is caught, we draw a black pixel ourselves
    // This function applies both kernels in one go, and only looks for alpha
    // It is ment to work on a filtered image, that only has 0 or 255 alpha values
    // So filter any image that has opacity gradient inside

    QImage* result = new QImage( *iImage );

    unsigned int index = 0;
    const uchar* originData = iImage->bits();
    uchar* outputData       = result->bits();

    int verticalValues[ 9 ]     = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
    int horizontalValues[ 9 ]   = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
    QGenericMatrix< 3, 3, int > vKernel( verticalValues );
    QGenericMatrix< 3, 3, int > hKernel( horizontalValues );

    for( unsigned int y = 1; y < iImage->height() - 1 ; ++y )
    {
        for( unsigned int x = 1; x < iImage->width() - 1; ++x )
        {
            index = y * iImage->bytesPerLine() + x * 4;

            if( originData[ index + 3 ] == 0 )
                continue;

            int tl = index - iImage->bytesPerLine() - 4;
            int top = index - iImage->bytesPerLine();
            int tr = index - iImage->bytesPerLine() + 4;

            int left = index - 4;
            int right = index + 4;

            int bl = index + iImage->bytesPerLine() - 4;
            int bottom = index + iImage->bytesPerLine();
            int br = index + iImage->bytesPerLine() + 4;

            int alphaConvolV =        (originData [ tl       +3 ] * vKernel( 0, 0 )
                                      + originData[ top      +3 ] * vKernel( 1, 0 )
                                      + originData[ tr       +3 ] * vKernel( 2, 0 )
                                      + originData[ left     +3 ] * vKernel( 0, 1 )
                                      + originData[ index    +3 ] * vKernel( 1, 1 )
                                      + originData[ right    +3 ] * vKernel( 2, 1 )
                                      + originData[ bl       +3 ] * vKernel( 0, 2 )
                                      + originData[ bottom   +3 ] * vKernel( 1, 2 )
                                      + originData[ br       +3 ] * vKernel( 2, 2 ) );

            int alphaConvolH =        (originData [ tl       +3 ] * hKernel( 0, 0 )
                                      + originData[ top      +3 ] * hKernel( 1, 0 )
                                      + originData[ tr       +3 ] * hKernel( 2, 0 )
                                      + originData[ left     +3 ] * hKernel( 0, 1 )
                                      + originData[ index    +3 ] * hKernel( 1, 1 )
                                      + originData[ right    +3 ] * hKernel( 2, 1 )
                                      + originData[ bl       +3 ] * hKernel( 0, 2 )
                                      + originData[ bottom   +3 ] * hKernel( 1, 2 )
                                      + originData[ br       +3 ] * hKernel( 2, 2 ) );

            int alphaConvol = 0;
            if( alphaConvolV != 0 || alphaConvolH != 0 )
                alphaConvol = 255;

            if( originData[ index + 3 ] == 0 )
                alphaConvol = 0;

            outputData[ index ]   = 0;
            outputData[ index+1 ] = 0;
            outputData[ index+2 ] = 0;
            outputData[ index+3 ] = alphaConvol;
        }
    }

    return  result;
}

