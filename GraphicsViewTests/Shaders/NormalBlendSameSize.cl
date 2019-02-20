
void
kernel NormalBlendSameSize( global const uchar* iSource, global uchar* iDestination, int BPL )
{
    int pixelIndex = get_global_id( 0 ) * 4;
    int transparencyAmountInverse = 255 - iSource[ pixelIndex + 3 ];
    iDestination[ pixelIndex + 0 ] = iSource[ pixelIndex + 0 ] + iDestination[ pixelIndex + 0 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 1 ] = iSource[ pixelIndex + 1 ] + iDestination[ pixelIndex + 1 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 2 ] = iSource[ pixelIndex + 2 ] + iDestination[ pixelIndex + 2 ] * transparencyAmountInverse / 255;
    iDestination[ pixelIndex + 3 ] = iSource[ pixelIndex + 3 ] + iDestination[ pixelIndex + 3 ] * transparencyAmountInverse / 255;
}
