
"void "
"kernel NoneBlendPixelToImage( global uchar* iDestination, int r, int g, int b, int a, int BPL )"
"{"
"    int pixX = get_global_id(0);"
"    int pixY = get_global_id(1);"
"    int pixelIndex = pixY * BPL + pixX * 4;"

"    iDestination[ pixelIndex + 0 ] = r;"
"    iDestination[ pixelIndex + 1 ] = g;"
"    iDestination[ pixelIndex + 2 ] = b;"
"    iDestination[ pixelIndex + 3 ] = a;"
"}"
