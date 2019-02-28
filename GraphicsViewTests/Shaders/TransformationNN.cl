typedef struct sPoint
{
    int mX;
    int mY;
} sPoint;


sPoint
local ApplyTransfo( float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8, float m9,
                    int iX, int iY )
{
    sPoint p;
    p.mX = iX * m1 + iY * m4 + m7;
    p.mY = iX * m2 + iY * m5 + m8;

    return  p;
}



void
kernel TransformationNN( global const unsigned char* iInput, global unsigned char* oOutput, int iInputBPL, int iOutputBPL,
                            int iOutputRight, int iOutputBottom,
                            int iOriginX, int iOriginY,
                            int iInputAreaLeft, int iInputAreaRight, int iInputAreaTop, int iInputAreaBottom,
                            float mI1, float mI2, float mI3, float mI4, float mI5, float mI6, float mI7, float mI8, float mI9  )
{
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);
    if( pixX >= iOutputRight || pixY >= iOutputBottom )
        return;

    int pixelIndex = pixY * iOutputBPL + pixX * 4;

    sPoint xyMapped = ApplyTransfo( mI1, mI2, mI3, mI4, mI5, mI6, mI7, mI8, mI9, pixX, pixY );

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


