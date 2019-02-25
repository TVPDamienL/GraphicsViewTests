
// Can't redefine a struct in one same cl::Program::Source
// typedef struct sPoint
// {
//     int mX;
//     int mY;
// } sPoint;


// sPoint
// local ApplyTransfo( global const float* iTransfo, int iX, int iY )
// {
//     sPoint p;
//     p.mX = iX * iTransfo[ 0 ] + iX * iTransfo[ 3 ] + iX * iTransfo[ 6 ];
//     p.mY = iY * iTransfo[ 1 ] + iY * iTransfo[ 4 ] + iY * iTransfo[ 7 ];

//     return  p;
// }



void
kernel Tests( global const float* iTransfo )
{
    sPoint ssss = ApplyTransfo( iTransfo, 10, 10 );
    // printf("Point : %d - %d\n", ssss.x, ssss.y );
}