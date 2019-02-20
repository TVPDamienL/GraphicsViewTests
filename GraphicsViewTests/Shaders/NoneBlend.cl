void
kernel NoneBlend( global const uchar* iSource, global uchar* iDestination, int BPL )
{
    int pixX = get_global_id(0);
    int pixY = get_global_id(1);
    int pixelIndex = pixY * BPL + pixX * 4;
    iDestination[ pixelIndex + 0 ] = iSource[ pixelIndex + 0 ];
    iDestination[ pixelIndex + 1 ] = iSource[ pixelIndex + 1 ];
    iDestination[ pixelIndex + 2 ] = iSource[ pixelIndex + 2 ];
    iDestination[ pixelIndex + 3 ] = iSource[ pixelIndex + 3 ];
}
