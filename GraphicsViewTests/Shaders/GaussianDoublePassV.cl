void
kernel GaussianDoublePassV( global const unsigned char* image, int width, int height, int bytesPerLine, global const double* gaussKernel, int kernelDim, global unsigned char* output )
{
   int pixelIndex = get_global_id(0) * 4;
   int x = pixelIndex % bytesPerLine / 4;
   int y = pixelIndex / bytesPerLine;
   int kernelCenter = kernelDim / 2;
   double rSum = 0;
   double gSum = 0;
   double bSum = 0;
   double aSum = 0;
   double kernelApplyed = 0;
   for( int kernY = 0; kernY < kernelDim; ++kernY )
   {
       int offY = kernY - kernelCenter;
       int lookupX = x;
       int lookupY = y + offY;
       if( lookupY < 0 || lookupY >= height )
           continue;
       int pixelIndex2 = lookupY * bytesPerLine + lookupX * 4;
       double kernelValue = gaussKernel[ kernY ];
       bSum += image[ pixelIndex2 + 0 ] * kernelValue;
       gSum += image[ pixelIndex2 + 1 ] * kernelValue;
       rSum += image[ pixelIndex2 + 2 ] * kernelValue;
       aSum += image[ pixelIndex2 + 3 ] * kernelValue;
       kernelApplyed += kernelValue;
   }
   output[pixelIndex]      = bSum/kernelApplyed;
   output[pixelIndex+1]    = gSum/kernelApplyed;
   output[pixelIndex+2]    = rSum/kernelApplyed;
   output[pixelIndex+3]    = aSum/kernelApplyed;
}
