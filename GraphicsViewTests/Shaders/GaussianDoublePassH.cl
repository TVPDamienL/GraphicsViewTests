void
kernel GaussianDoublePassH( global const unsigned char* image, int width, int height, int bytesPerLine, global const double* gaussKernel, int kernelDim, global unsigned char* output )
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
   for( int kernX = 0; kernX < kernelDim; ++kernX )
   {
       int offX = kernX - kernelCenter;
       int lookupX = x + offX;
       int lookupY = y;
       if( lookupX < 0 || lookupX >= width )
           continue;
       int pixelIndex2 = lookupY * bytesPerLine + lookupX * 4;
       double kernelValue = gaussKernel[ kernX ];
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