#include "GPUForThisApp.h"

#include <QDebug>
#include "BenchmarkStuff.h"


static GPUForThisApp* sGPU = 0;

// static
GPUForThisApp*
GPUForThisApp::Instance()
{
    if( !sGPU )
        sGPU = new GPUForThisApp();

    return  sGPU;
}


GPUForThisApp::~GPUForThisApp()
{
}


GPUForThisApp::GPUForThisApp() :
    GPUController()
{
    RegisterShader( "Shaders/GaussianDoublePassH.cl" );
    RegisterShader( "Shaders/GaussianDoublePassV.cl" );
    RegisterShader( "Shaders/NormalBlend.cl" );
    RegisterShader( "Shaders/NormalBlendMore.cl" );
    RegisterShader( "Shaders/NormalBlendSameSize.cl" );
    RegisterShader( "Shaders/NormalBlendPixelToImage.cl" );
    RegisterShader( "Shaders/Sum.cu" );
    RegisterShader( "Shaders/NoneBlendPixelToImage.cl" );
    RegisterShader( "Shaders/Tests.cl" );

    if( BuildProgram() != CL_SUCCESS )
        return;

    BuildKernel( "GaussianDoublePassH" );
    BuildKernel( "GaussianDoublePassV" );
    BuildKernel( "NormalBlend" );
    BuildKernel( "NormalBlendMore" );
    BuildKernel( "NormalBlendSameSize" );
    BuildKernel( "NormalBlendPixelToImage" );
    BuildKernel( "NoneBlend" );
    BuildKernel( "NoneBlendPixelToImage" );
    BuildKernel( "Tests" );
}



QImage*
GPUForThisApp::GaussianBlurGPU1D2Pass( QImage* image )
{
    const int kernelSize = 101;
    int kernelCenter = kernelSize/2;
    double gaussKernel[] =
    {
        0.007039, 0.00718	,0.007321	,0.007461	,0.007601	,0.007741	,0.00788	,0.008018	,0.008155	,0.008292	,0.008427	,0.008561	,0.008694,.008826,.008956,.009084,.00921,.009334,.009456,.009576,.009694,.009809,.009921,.010031,.010138,.010242,.010343,.01044,.010535,.010626,.010713,.010797,.010877,.010954,.011026,.011095,.011159,.01122,.011276,.011328,.011376,.011419,.011458,.011492,.011522,.011547,.011568,.011584,.011596,.011603,.011605,.011603,.011596,.011584,.011568,.011547,.011522,.011492,.011458,.011419,.011376,.011328,.011276,.01122,.011159,.011095,.011026,.010954,.010877,.010797,.010713,.010626,.010535,.01044,.010343,.010242,.010138,.010031,.009921,.009809,.009694,.009576,.009456,.009334,.00921,.009084,.008956,.008826,.008694,.008561,.008427,.008292,.008155,.008018,.00788,.007741,.007601,.007461,.007321,.00718,.007039
    };

    // Datas and buffers
    uchar* imageData = image->bits();
    uint totalSize = image->sizeInBytes();
    uint byteSizeImage = sizeof(uchar) * totalSize;
    uint byteSizeKernel = sizeof(double) * kernelSize;
    QImage* output = new QImage( image->width(), image->height(), QImage::Format_ARGB32_Premultiplied );

    // Local buffers
    cl::Buffer memImageData( *mContext, CL_MEM_READ_WRITE, byteSizeImage );
    cl::Buffer memKernel( *mContext, CL_MEM_READ_WRITE, byteSizeKernel );
    cl::Buffer memBuffer( *mContext, CL_MEM_READ_WRITE, byteSizeImage );
    cl::Buffer memOutput( *mContext, CL_MEM_WRITE_ONLY, byteSizeImage );

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue( *mContext, mDevice );

    cl::Kernel* kernel_GaussV = GetKernel( "GaussianDoublePassV" );
    kernel_GaussV->setArg( 0, memImageData );
    kernel_GaussV->setArg( 1, image->width() );
    kernel_GaussV->setArg( 2, image->height() );
    kernel_GaussV->setArg( 3, image->bytesPerLine() );
    kernel_GaussV->setArg( 4, memKernel );
    kernel_GaussV->setArg( 5, kernelSize );
    kernel_GaussV->setArg( 6, memBuffer );

    //alternative way to run the kernel
    cl::Kernel* kernel_GaussH = GetKernel( "GaussianDoublePassH" );
    kernel_GaussH->setArg( 0, memBuffer );
    kernel_GaussH->setArg( 1, image->width() );
    kernel_GaussH->setArg( 2, image->height() );
    kernel_GaussH->setArg( 3, image->bytesPerLine() );
    kernel_GaussH->setArg( 4, memKernel );
    kernel_GaussH->setArg( 5, kernelSize );
    kernel_GaussH->setArg( 6, memOutput );


    // Buffer setup
    queue.enqueueWriteBuffer( memKernel, CL_TRUE, 0, byteSizeKernel, gaussKernel );

    queue.enqueueWriteBuffer( memImageData, CL_TRUE, 0, byteSizeImage, imageData );

    // Vertical call
    queue.enqueueNDRangeKernel( *kernel_GaussV, cl::NullRange, cl::NDRange(totalSize/4), cl::NullRange );
    //queue.finish();

    // Horizontal call
    queue.enqueueNDRangeKernel( *kernel_GaussH, cl::NullRange, cl::NDRange(totalSize/4), cl::NullRange );
    //queue.finish();

    queue.enqueueReadBuffer( memOutput, CL_TRUE, 0, byteSizeImage, output->bits() );

    return  output;
}







// Assumes source && dest have same size
void
GPUForThisApp::BlendImageSameSizesGPU( QImage* source, QImage* destination, const QRect& area, int iBlendingMode )
{
    // Datas and buffers
    uchar* srcData = source->bits();
    uint sourceTotalSize = source->sizeInBytes();
    uint byteSizeSrcImage = sizeof(uchar) * sourceTotalSize;
    uchar* dstData = destination->bits();
    uint dstTotalSize = destination->sizeInBytes();
    uint byteSizeDstImage = sizeof(uchar) * dstTotalSize;

    // create buffers on the device
    cl::Buffer memSrcImage( *mContext, CL_MEM_READ_ONLY, byteSizeSrcImage );
    cl::Buffer memDstImage( *mContext, CL_MEM_READ_WRITE, byteSizeDstImage );

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue( *mContext, mDevice );

    cl::Kernel* kernelBlendNormal = GetKernel( "NormalBlend" );
    kernelBlendNormal->setArg( 0, memSrcImage );
    kernelBlendNormal->setArg( 1, memDstImage );
    kernelBlendNormal->setArg( 2, destination->bytesPerLine() );

    cl::Kernel* kernelBlendNone = GetKernel( "NoneBlend" );
    kernelBlendNone->setArg( 0, memSrcImage );
    kernelBlendNone->setArg( 1, memDstImage );
    kernelBlendNone->setArg( 2, destination->bytesPerLine() );



    // Now, we can do this, as there are no more inline to deal with !
    cl::Kernel* theKernel = 0;
    if( iBlendingMode == 0 )
        theKernel = kernelBlendNormal;
    else
        theKernel = kernelBlendNone;


    // Road to GPU
    queue.enqueueWriteBuffer( memSrcImage, CL_TRUE, 0, sourceTotalSize, srcData );
    queue.enqueueWriteBuffer( memDstImage, CL_TRUE, 0, dstTotalSize, dstData );

    // area
    const int minX = area.left();
    const int maxX = area.right();
    const int minY = area.top();
    const int maxY = area.bottom();

    // Clipping
    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= destination->width() ? destination->width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= destination->height() ? destination->height() - 1 : maxY;
    const int width = endingX - startingX;
    const int height = endingY - startingY;

    CL_INVALID_PROGRAM_EXECUTABLE;
    // Vertical call
    cl_int error = queue.enqueueNDRangeKernel( *theKernel, cl::NDRange( startingX, startingY ), cl::NDRange( width + 1, height + 1 ), cl::NullRange );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << error;
    queue.finish();

    queue.enqueueReadBuffer( memDstImage, CL_TRUE, 0, byteSizeDstImage, destination->bits() );
}


void GPUForThisApp::FillImageGPU( QImage * destination, const QRect & area, const QColor & iColor, int iBlendingMode )
{
    // Datas and buffers
    uchar* dstData = destination->bits();
    uint dstTotalSize = destination->sizeInBytes();
    uint byteSizeDstImage = sizeof(uchar) * dstTotalSize;

    // create buffers on the device
    cl::Buffer memDstImage( *mContext, CL_MEM_READ_WRITE, byteSizeDstImage );

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue( *mContext, mDevice );

    cl::Kernel* kernelBlendNormal = GetKernel( "NormalBlendPixelToImage" );
    kernelBlendNormal->setArg( 0, memDstImage );
    kernelBlendNormal->setArg( 1, iColor.blue() );
    kernelBlendNormal->setArg( 2, iColor.green() );
    kernelBlendNormal->setArg( 3, iColor.red() );
    kernelBlendNormal->setArg( 4, iColor.alpha() );
    kernelBlendNormal->setArg( 5, destination->bytesPerLine() );

    cl::Kernel* kernelBlendNone = GetKernel( "NoneBlendPixelToImage" );
    kernelBlendNone->setArg( 0, memDstImage );
    kernelBlendNone->setArg( 1, iColor.blue() );
    kernelBlendNone->setArg( 2, iColor.green() );
    kernelBlendNone->setArg( 3, iColor.red() );
    kernelBlendNone->setArg( 4, iColor.alpha() );
    kernelBlendNone->setArg( 5, destination->bytesPerLine() );

    // Now, we can do this, as there are no more inline to deal with !
    cl::Kernel* theKernel = 0;
    if( iBlendingMode == 0 )
        theKernel = kernelBlendNormal;
    else
        theKernel = kernelBlendNone;


    // Road to GPU
    queue.enqueueWriteBuffer( memDstImage, CL_TRUE, 0, dstTotalSize, dstData );

    // area
    const int minX = area.left();
    const int maxX = area.right();
    const int minY = area.top();
    const int maxY = area.bottom();

    // Clipping
    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= destination->width() ? destination->width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= destination->height() ? destination->height() - 1 : maxY;
    const int width = endingX - startingX;
    const int height = endingY - startingY;

    //CL_INVALID_PROGRAM_EXECUTABLE;
    // Vertical call
    cl_int error = queue.enqueueNDRangeKernel( *theKernel, cl::NDRange( startingX, startingY ), cl::NDRange( width + 1, height + 1 ), cl::NullRange );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << error;
    queue.finish();

    queue.enqueueReadBuffer( memDstImage, CL_TRUE, 0, byteSizeDstImage, destination->bits() );
}


void
GPUForThisApp::SetLayersByteSize( uint iSize )
{
    mLayerByteSize = iSize;

    mOutputBuffer = NewBuffer( mLayerByteSize, CL_MEM_READ_WRITE );
}


void
GPUForThisApp::SetLayersBytePerLine( uint iSize )
{
    mLayerBytePerLine = iSize;
}


void
GPUForThisApp::LoadLayerToGPU( QImage * iLayerImage )
{
    cl::Buffer* layer = NewBuffer( iLayerImage->sizeInBytes(), CL_MEM_READ_WRITE );

    cl::CommandQueue queue( *mContext, mDevice );
    queue.enqueueWriteBuffer( *layer, CL_FALSE, 0, iLayerImage->sizeInBytes(), iLayerImage->bits() );

    mLayers.push_back( layer );
}

void
GPUForThisApp::PerformLayerCompositing( QImage* oResult, const QRect& iDirtyArea )
{
    if( iDirtyArea.isEmpty() )
        return;

    // Align dirty area to fit 32*32
    QRect areaAligned = iDirtyArea;
    areaAligned.setWidth( (areaAligned.width() | 31) + 1 );
    areaAligned.setHeight( (areaAligned.height() | 31) + 1 );

    if( areaAligned.right() >= oResult->width() )
    {
        areaAligned.moveLeft( oResult->width() - areaAligned.width() ); // It's a move to, not move by
    }
    if( areaAligned.bottom() >= oResult->height() )
    {
        areaAligned.moveTop( oResult->height() - areaAligned.height() ); // It's a move to, not move by
    }

    cl::Kernel* kernelBlendNormal = GetKernel( "NormalBlend" );
    cl::Kernel* kernelFillNone = GetKernel( "NoneBlendPixelToImage" );

    // Fill Transparent
    kernelFillNone->setArg( 0, *mOutputBuffer );
    kernelFillNone->setArg( 1, 0 );
    kernelFillNone->setArg( 2, 255 );
    kernelFillNone->setArg( 3, 0 );
    kernelFillNone->setArg( 4, 255 );
    kernelFillNone->setArg( 5, mLayerBytePerLine );

    cl_int error = mQueue->enqueueNDRangeKernel( *kernelFillNone, cl::NDRange( iDirtyArea.left(), iDirtyArea.top() ), cl::NDRange( iDirtyArea.width(), iDirtyArea.height() ), cl::NullRange );
    mQueue->finish();

    kernelBlendNormal->setArg( 1, *mOutputBuffer );
    kernelBlendNormal->setArg( 2, mLayerBytePerLine );

    qDebug() << " Area : " << areaAligned;
    //BENCHSTART
    // Composite all layers
    for( int i = 0; i < mLayers.size(); ++i )
    {
        kernelBlendNormal->setArg( 0, *mLayers[ i ] );

        cl_int error = mQueue->enqueueNDRangeKernel( *kernelBlendNormal, cl::NDRange( areaAligned.left(), areaAligned.top() ), cl::NDRange( areaAligned.width(), areaAligned.height() ), cl::NDRange( 32, 32 ) );
        if( error != CL_SUCCESS )
            qDebug() << "FAILED KERNEL : " << error;
        mQueue->finish();
    }
    //BENCHEND

    cl::size_t<3> offset; // In byte too
    offset[ 0 ] = areaAligned.left() * 4;
    offset[ 1 ] = areaAligned.top();

    cl::size_t<3> size; // In bytes
    size[ 0 ] = areaAligned.width()  *4; // Pixel is 4 bytes wide
    size[ 1 ] = areaAligned.height() *1; // 1 byte high
    size[ 2 ] = 1; // 1 Byte deep

    mQueue->enqueueReadBufferRect( *mOutputBuffer, CL_TRUE, offset, offset, size, mLayerBytePerLine, 0, mLayerBytePerLine, 0, oResult->bits() );
}
