
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
}