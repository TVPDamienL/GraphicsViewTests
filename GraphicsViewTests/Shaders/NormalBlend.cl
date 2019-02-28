
void
kernel NormalBlend( global const uchar* iSource, global uchar* iDestination, int BPL )
{
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);

    int pixelIndex = pixY * BPL + pixX * 4;

    int transparencyAmountInverse = 255 - iSource[ pixelIndex + 3 ];
    iDestination[ pixelIndex + 0 ] = iSource[ pixelIndex + 0 ]  + iDestination[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 1 ] = iSource[ pixelIndex + 1 ]  + iDestination[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 2 ] = iSource[ pixelIndex + 2 ]  + iDestination[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 3 ] = iSource[ pixelIndex + 3 ]  + iDestination[ pixelIndex + 3 ] * transparencyAmountInverse / 255;
}


void
kernel NormalBlendNotSameSizes( global const uchar* iSource, int iSrcBPL,
                                global uchar* iDestination, int iDstBPL,
                                int xOff, int yOff,
                                int xLimit, int yLimit )
{
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);

    if( pixX >= xLimit || pixY >= yLimit )
        return;

    int dstIndex = pixY * iDstBPL + pixX * 4;
    int srcIndex = (pixY - yOff) * iSrcBPL + (pixX - xOff) * 4;

    int transparencyAmountInverse = 255 - iSource[ srcIndex + 3 ];
    iDestination[ dstIndex + 0 ] = iSource[ srcIndex + 0 ]  + iDestination[ dstIndex + 0 ] * transparencyAmountInverse / 255;
    iDestination[ dstIndex + 1 ] = iSource[ srcIndex + 1 ]  + iDestination[ dstIndex + 1 ] * transparencyAmountInverse / 255;
    iDestination[ dstIndex + 2 ] = iSource[ srcIndex + 2 ]  + iDestination[ dstIndex + 2 ] * transparencyAmountInverse / 255;
    iDestination[ dstIndex + 3 ] = iSource[ srcIndex + 3 ]  + iDestination[ dstIndex + 3 ] * transparencyAmountInverse / 255;


// This is using *uchar4 instead of *uchar
// Gains no performance
//     int pixX = get_global_id(0);
//     int pixY = get_global_id(1);

//     if( pixX >= xLimit || pixY >= yLimit )
//         return;

//     int dstIndex = pixY * 2048 + pixX;
//     int srcIndex = (pixY - yOff) * 2048 + (pixX - xOff);

//     int alpha = iSource[ srcIndex ].w;
//     int transparencyAmountInverse = 255 - alpha;
//     iDestination[ dstIndex ].x = iSource[ srcIndex ].x  + iDestination[ dstIndex ].x * transparencyAmountInverse / 255;
//     iDestination[ dstIndex ].y = iSource[ srcIndex ].y  + iDestination[ dstIndex ].y * transparencyAmountInverse / 255;
//     iDestination[ dstIndex ].z = iSource[ srcIndex ].z  + iDestination[ dstIndex ].z * transparencyAmountInverse / 255;
//     iDestination[ dstIndex ].w = alpha                  + iDestination[ dstIndex ].w * transparencyAmountInverse / 255;


    // Hardest (suposedly) optimizations
    // int localSize = get_local_size( 0 );
    // int workGroupID = get_group_id( 0 );
    // int localID = get_local_id( 0 );
    // int componentNumber = localID % 4;
    // int localPxCut = localID / 4;
    // int localPixel = localPxCut * 4;
    // int localPixel2 = localPixel + localSize;
    // int localPixel3 = localPixel2 + localSize;
    // int localPixel4 = localPixel3 + localSize;

    // int pixX = workGroupID * localSize;
    // int pixY = get_global_id(1);

    // int destIndex = localPixel + pixY * iDstBPL + pixX * 4;
    // int destIndex2 = destIndex + localSize;
    // int destIndex3 = destIndex2 + localSize;
    // int destIndex4 = destIndex3 + localSize;

    // int srcIndex = localPixel + (pixY - yOff) * iSrcBPL + (pixX - xOff) * 4;
    // int srcIndex2 = srcIndex + localSize;
    // int srcIndex3 = srcIndex2 + localSize;
    // int srcIndex4 = srcIndex3 + localSize;

    // local uchar inputComps[ 1024 ];
    // inputComps[ localPixel + componentNumber ] = iSource[ srcIndex + componentNumber ];
    // inputComps[ localPixel2 + componentNumber ] = iSource[ srcIndex2 + componentNumber ];
    // inputComps[ localPixel3 + componentNumber ] = iSource[ srcIndex3 + componentNumber ];
    // inputComps[ localPixel4 + componentNumber ] = iSource[ srcIndex4 + componentNumber ];

    // local uchar outputComps[ 1024 ];
    // outputComps[ localPixel + componentNumber ] =  iDestination[ destIndex + componentNumber ];
    // outputComps[ localPixel2 + componentNumber ] = iDestination[ destIndex2 + componentNumber ];
    // outputComps[ localPixel3 + componentNumber ] = iDestination[ destIndex3 + componentNumber ];
    // outputComps[ localPixel4 + componentNumber ] = iDestination[ destIndex4 + componentNumber ];


    // barrier( CLK_LOCAL_MEM_FENCE  );


    // int transparencyAmountInverse = 255 - inputComps[ localPixel + 3 ];
    // int transparencyAmountInverse2 = 255 - inputComps[ localPixel2 + 3 ];
    // int transparencyAmountInverse3 = 255 - inputComps[ localPixel3 + 3 ];
    // int transparencyAmountInverse4 = 255 - inputComps[ localPixel4 + 3 ];

    // // output[ pixelIndex + componentNumber ] = inputComps[ localPixel + componentNumber ]  + outputComps[ localPixel + componentNumber ] * transparencyAmountInverse / 255;
    // iDestination[ destIndex + componentNumber ] = inputComps[ localPixel + componentNumber ]  + outputComps[ localPixel + componentNumber ] * transparencyAmountInverse / 255;
    // iDestination[ destIndex2 + componentNumber ] = inputComps[ localPixel2 + componentNumber ]  + outputComps[ localPixel2 + componentNumber ] * transparencyAmountInverse2 / 255;
    // iDestination[ destIndex3 + componentNumber ] = inputComps[ localPixel3 + componentNumber ]  + outputComps[ localPixel3 + componentNumber ] * transparencyAmountInverse3 / 255;
    // iDestination[ destIndex4 + componentNumber ] = inputComps[ localPixel4 + componentNumber ]  + outputComps[ localPixel4 + componentNumber ] * transparencyAmountInverse4 / 255;

}