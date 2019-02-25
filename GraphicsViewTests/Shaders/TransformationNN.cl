typedef struct sPoint
{
    int mX;
    int mY;
} sPoint;


sPoint
local ApplyTransfo( global const float* iTransfo, int iX, int iY )
{
    sPoint p;
    p.mX = iX * iTransfo[ 0 ] + iY * iTransfo[ 3 ] + iTransfo[ 6 ];
    p.mY = iX * iTransfo[ 1 ] + iY * iTransfo[ 4 ] + iTransfo[ 7 ];

    return  p;
}



void
kernel TransformationNN( global const unsigned char* iInput, global unsigned char* oOutput, int iInputBPL, int iOutputBPL,
                            int iOutputRight, int iOutputBottom,
                            global const float* iTransfo, global const float* iTransfoInverse,
                            int iOriginX, int iOriginY,
                            int iInputAreaLeft, int iInputAreaRight, int iInputAreaTop, int iInputAreaBottom  )
{
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);
    if( pixX >= iOutputRight || pixY >= iOutputBottom )
        return;

    int pixelIndex = pixY * iOutputBPL + pixX * 4;

    sPoint xyMapped = ApplyTransfo( iTransfoInverse, pixX, pixY );

    // If inputArea doesn't contains xyMapped
    if( xyMapped.mX < iInputAreaLeft || xyMapped.mX > iInputAreaRight
        || xyMapped.mY < iInputAreaTop || xyMapped.mY > iInputAreaBottom )
    {
        oOutput[ pixelIndex + 0 ] = 0;
        oOutput[ pixelIndex + 1 ] = 0;
        oOutput[ pixelIndex + 2 ] = 0;
        oOutput[ pixelIndex + 3 ] = 0;
    }
    else
    {
        int inputIndex = (xyMapped.mY - iOriginY) * iInputBPL + (xyMapped.mX - iOriginX) * 4;

        oOutput[ pixelIndex + 0 ] = iInput[ inputIndex + 0 ];
        oOutput[ pixelIndex + 1 ] = iInput[ inputIndex + 1 ];
        oOutput[ pixelIndex + 2 ] = iInput[ inputIndex + 2 ];
        oOutput[ pixelIndex + 3 ] = iInput[ inputIndex + 3 ];
    }
}
