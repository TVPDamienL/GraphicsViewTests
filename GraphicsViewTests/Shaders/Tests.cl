

void
kernel Tests( global const uchar* image, global uchar* output, int BPL, global const float* krnl )
{
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

    // int pixelIndex = localPixel + pixY * BPL + pixX*4;
    // int pixelIndex2 = pixelIndex + localSize;
    // int pixelIndex3 = pixelIndex2 + localSize;
    // int pixelIndex4 = pixelIndex3 + localSize;

    // local uchar inputComps[ 1024 ];
    // inputComps[ localPixel + componentNumber ] = image[ pixelIndex + componentNumber ];
    // inputComps[ localPixel2 + componentNumber ] = image[ pixelIndex2 + componentNumber ];
    // inputComps[ localPixel3 + componentNumber ] = image[ pixelIndex3 + componentNumber ];
    // inputComps[ localPixel4 + componentNumber ] = image[ pixelIndex4 + componentNumber ];

    // local uchar outputComps[ 1024 ];
    // outputComps[ localPixel + componentNumber ] = output[ pixelIndex + componentNumber ];
    // outputComps[ localPixel2 + componentNumber ] = output[ pixelIndex2 + componentNumber ];
    // outputComps[ localPixel3 + componentNumber ] = output[ pixelIndex3 + componentNumber ];
    // outputComps[ localPixel4 + componentNumber ] = output[ pixelIndex4 + componentNumber ];


    // barrier( CLK_LOCAL_MEM_FENCE  );


    // int transparencyAmountInverse = 255 - inputComps[ localPixel + 3 ];
    // int transparencyAmountInverse2 = 255 - inputComps[ localPixel2 + 3 ];
    // int transparencyAmountInverse3 = 255 - inputComps[ localPixel3 + 3 ];
    // int transparencyAmountInverse4 = 255 - inputComps[ localPixel4 + 3 ];

    // // output[ pixelIndex + componentNumber ] = inputComps[ localPixel + componentNumber ]  + outputComps[ localPixel + componentNumber ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + componentNumber ] = inputComps[ localPixel + componentNumber ]  + outputComps[ localPixel + componentNumber ] * transparencyAmountInverse / 255;
    // output[ pixelIndex2 + componentNumber ] = inputComps[ localPixel2 + componentNumber ]  + outputComps[ localPixel2 + componentNumber ] * transparencyAmountInverse2 / 255;
    // output[ pixelIndex3 + componentNumber ] = inputComps[ localPixel3 + componentNumber ]  + outputComps[ localPixel3 + componentNumber ] * transparencyAmountInverse3 / 255;
    // output[ pixelIndex4 + componentNumber ] = inputComps[ localPixel4 + componentNumber ]  + outputComps[ localPixel4 + componentNumber ] * transparencyAmountInverse4 / 255;


    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================


    // // Hardest (suposedly) optimizations
    // int localSize = get_local_size( 0 );
    // int workGroupID = get_group_id( 0 );

    // int localID = get_local_id( 0 );
    // int localPixel = localID * 4;

    // int pixX = workGroupID * localSize;
    // int pixY = get_global_id(1);

    // int pixelIndex = localPixel + pixY * BPL + pixX*4;

    // local uchar inputComps[ 1024 ];
    // inputComps[ localPixel + 0 ] = image[ pixelIndex + 0 ];
    // inputComps[ localPixel + 1 ] = image[ pixelIndex + 1 ];
    // inputComps[ localPixel + 2 ] = image[ pixelIndex + 2 ];
    // inputComps[ localPixel + 3 ] = image[ pixelIndex + 3 ];

    // local uchar outputComps[ 1024 ];
    // outputComps[ localPixel + 0 ] = output[ pixelIndex + 0 ];
    // outputComps[ localPixel + 1 ] = output[ pixelIndex + 1 ];
    // outputComps[ localPixel + 2 ] = output[ pixelIndex + 2 ];
    // outputComps[ localPixel + 3 ] = output[ pixelIndex + 3 ];


    // barrier( CLK_LOCAL_MEM_FENCE  );


    // int transparencyAmountInverse = 255 - inputComps[ localPixel + 3 ];

    // output[ pixelIndex + 0 ] = inputComps[ localPixel + 0 ]  + outputComps[ localPixel + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 1 ] = inputComps[ localPixel + 1 ]  + outputComps[ localPixel + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 2 ] = inputComps[ localPixel + 2 ]  + outputComps[ localPixel + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 3 ] = inputComps[ localPixel + 3 ]  + outputComps[ localPixel + 3 ] * transparencyAmountInverse / 255;


    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================


    // // NO optimizations, basic kernel
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);
    int pixelIndex = pixY * BPL + pixX*4;

    int transparencyAmountInverse = 255 - image[ pixelIndex + 3 ];
    output[ pixelIndex + 0 ] = image[ pixelIndex + 0 ]  + output[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    output[ pixelIndex + 1 ] = image[ pixelIndex + 1 ]  + output[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    output[ pixelIndex + 2 ] = image[ pixelIndex + 2 ]  + output[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    output[ pixelIndex + 3 ] = image[ pixelIndex + 3 ]  + output[ pixelIndex + 3 ] * transparencyAmountInverse / 255;


    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================
    // ======================================================


    // NO optimizations, basic kernel
    // int pixX = get_global_id(0);
    // int pixY = get_global_id(1);
    // int pixelIndex = pixY * BPL + pixX*4;

    // int transparencyAmountInverse = 255 - image[ pixelIndex + 3 ];
    // output[ pixelIndex + 0 ] = image[ pixelIndex + 0 ]  + output[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 1 ] = image[ pixelIndex + 1 ]  + output[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 2 ] = image[ pixelIndex + 2 ]  + output[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 3 ] = image[ pixelIndex + 3 ]  + output[ pixelIndex + 3 ] * transparencyAmountInverse / 255;




    // int transparencyAmountInverse = 255 - image[ pixelIndex + 3 ];

    // int a  = image[ pixelIndex + 0 ]    + output[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    // int b  = image[ pixelIndex + 1 ]    + output[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    // int c  = image[ pixelIndex + 2 ]    + output[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    // int d  = image[ pixelIndex + 3 ]    + output[ pixelIndex + 3 ] * transparencyAmountInverse / 255;
    // int a2  = image[ pixelIndex2 + 0 ]  + output[ pixelIndex2 + 0 ] * transparencyAmountInverse / 255;
    // int b2  = image[ pixelIndex2 + 1 ]  + output[ pixelIndex2 + 1 ] * transparencyAmountInverse / 255;
    // int c2  = image[ pixelIndex2 + 2 ]  + output[ pixelIndex2 + 2 ] * transparencyAmountInverse / 255;
    // int d2  = image[ pixelIndex2 + 3 ]  + output[ pixelIndex2 + 3 ] * transparencyAmountInverse / 255;
    // int a3  = image[ pixelIndex3 + 0 ]  + output[ pixelIndex3 + 0 ] * transparencyAmountInverse / 255;
    // int b3  = image[ pixelIndex3 + 1 ]  + output[ pixelIndex3 + 1 ] * transparencyAmountInverse / 255;
    // int c3  = image[ pixelIndex3 + 2 ]  + output[ pixelIndex3 + 2 ] * transparencyAmountInverse / 255;
    // int d3  = image[ pixelIndex3 + 3 ]  + output[ pixelIndex3 + 3 ] * transparencyAmountInverse / 255;
    // int a4  = image[ pixelIndex4 + 0 ]  + output[ pixelIndex4 + 0 ] * transparencyAmountInverse / 255;
    // int b4  = image[ pixelIndex4 + 1 ]  + output[ pixelIndex4 + 1 ] * transparencyAmountInverse / 255;
    // int c4  = image[ pixelIndex4 + 2 ]  + output[ pixelIndex4 + 2 ] * transparencyAmountInverse / 255;
    // int d4  = image[ pixelIndex4 + 3 ]  + output[ pixelIndex4 + 3 ] * transparencyAmountInverse / 255;
    // int result = a + b + c + d;
    // transparencyAmountInverse /= result;
    // output[ pixelIndex + 0 ] = 255;
    // output[ pixelIndex + 1 ] = 255;
    // output[ pixelIndex + 2 ] = 255;
    // output[ pixelIndex + 3 ] = 255;
    // output[ pixelIndex2 + 0 ] = 255;
    // output[ pixelIndex2 + 1 ] = 255;
    // output[ pixelIndex2 + 2 ] = 255;
    // output[ pixelIndex2 + 3 ] = 255;
    // output[ pixelIndex3 + 0 ] = 255;
    // output[ pixelIndex3 + 1 ] = 255;
    // output[ pixelIndex3 + 2 ] = 255;
    // output[ pixelIndex3 + 3 ] = 255;
    // output[ pixelIndex4 + 0 ] = 255;
    // output[ pixelIndex4 + 1 ] = 255;
    // output[ pixelIndex4 + 2 ] = 255;
    // output[ pixelIndex4 + 3 ] = 255;
    // output[ pixelIndex + 0 ] = image[ pixelIndex + 0 ]  + output[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 1 ] = image[ pixelIndex + 1 ]  + output[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 2 ] = image[ pixelIndex + 2 ]  + output[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + 3 ] = image[ pixelIndex + 3 ]  + output[ pixelIndex + 3 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex2 + 0 ] = image[ pixelIndex2 + 0 ]  + output[ pixelIndex2 + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex2 + 1 ] = image[ pixelIndex2 + 1 ]  + output[ pixelIndex2 + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex2 + 2 ] = image[ pixelIndex2 + 2 ]  + output[ pixelIndex2 + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex2 + 3 ] = image[ pixelIndex2 + 3 ]  + output[ pixelIndex2 + 3 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex3 + 0 ] = image[ pixelIndex3 + 0 ]  + output[ pixelIndex3 + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex3 + 1 ] = image[ pixelIndex3 + 1 ]  + output[ pixelIndex3 + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex3 + 2 ] = image[ pixelIndex3 + 2 ]  + output[ pixelIndex3 + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex3 + 3 ] = image[ pixelIndex3 + 3 ]  + output[ pixelIndex3 + 3 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex4 + 0 ] = image[ pixelIndex4 + 0 ]  + output[ pixelIndex4 + 0 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex4 + 1 ] = image[ pixelIndex4 + 1 ]  + output[ pixelIndex4 + 1 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex4 + 2 ] = image[ pixelIndex4 + 2 ]  + output[ pixelIndex4 + 2 ] * transparencyAmountInverse / 255;
    // output[ pixelIndex4 + 3 ] = image[ pixelIndex4 + 3 ]  + output[ pixelIndex4 + 3 ] * transparencyAmountInverse / 255;


    // output[ pixelIndex + locX ] = image[ pixelIndex + locX ]  + output[ pixelIndex + locX ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + locX ] = image[ pixelIndex + locX ]  + output[ pixelIndex + locX ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + locX ] = image[ pixelIndex + locX ]  + output[ pixelIndex + locX ] * transparencyAmountInverse / 255;
    // output[ pixelIndex + locX ] = image[ pixelIndex + locX ]  + output[ pixelIndex + locX ] * transparencyAmountInverse / 255;
}