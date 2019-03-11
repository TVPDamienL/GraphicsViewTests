#include "GPUForThisApp.h"

#include <QDebug>
#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include <iostream>


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
    delete  mTransformationInverse;
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
    RegisterShader( "Shaders/TransformationNN.cl" );
    RegisterShader( "Shaders/Tests.cl" );

    if( BuildProgram() != CL_SUCCESS )
        return;

    BuildKernel( "GaussianDoublePassH" );
    BuildKernel( "GaussianDoublePassV" );
    BuildKernel( "NormalBlend" );
    BuildKernel( "NormalBlendNotSameSizes" );
    BuildKernel( "NormalBlendMore" );
    BuildKernel( "NormalBlendSameSize" );
    BuildKernel( "NormalBlendPixelToImage" );
    BuildKernel( "NoneBlend" );
    BuildKernel( "NoneBlendPixelToImage" );
    BuildKernel( "TransformationNN" );
    BuildKernel( "Tests" );

    mTransformationInverse = NewBuffer( sizeof(float)*9, CL_MEM_READ_ONLY );
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

    // Vertical call
    cl_int error = queue.enqueueNDRangeKernel( *theKernel, cl::NDRange( startingX, startingY ), cl::NDRange( width + 1, height + 1 ), cl::NullRange );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << error;
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

    cl_int error = queue.enqueueNDRangeKernel( *theKernel, cl::NDRange( startingX, startingY ), cl::NDRange( width + 1, height + 1 ), cl::NullRange );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << error;
    queue.finish();

    queue.enqueueReadBuffer( memDstImage, CL_TRUE, 0, byteSizeDstImage, destination->bits() );
}


void
GPUForThisApp::BlendImages( QImage * source, QImage * destination, const QRect & area, int iBlendingMode )
{
    auto src = NewBuffer( source->sizeInBytes(), CL_MEM_READ_WRITE );
    mQueue->enqueueWriteBuffer( *src, CL_FALSE, 0, source->sizeInBytes(), source->bits() );
    auto dst = NewBuffer( destination->sizeInBytes(), CL_MEM_READ_WRITE );
    mQueue->enqueueWriteBuffer( *dst, CL_FALSE, 0, destination->sizeInBytes(), destination->bits() );

    _BlendImage( src, source->rect(), source->bytesPerLine(), dst, destination->rect(), destination->bytesPerLine(), QPoint( 0, 0 ) );
    mQueue->enqueueReadBuffer( *dst, CL_TRUE, 0, destination->sizeInBytes(), destination->bits() );

    DeleteBuffer( src );
    DeleteBuffer( dst );
}


// GPU Version of standard methods


void
GPUForThisApp::_TransformNN( cl::Buffer* iInput, int iInputBPL, const QRect& iInputArea,
                             cl::Buffer* oOutput, int iOutputBPL, const QRect& iOutputArea,
                             const QTransform & iTransfo, const QPoint & iOrigin )
{
    auto  inverted = iTransfo.inverted();
    float transMatrixInverse[ 9 ] = { inverted.m11(),  inverted.m12(),  inverted.m13(),  inverted.m21(),  inverted.m22(),  inverted.m23(),  inverted.m31(),  inverted.m32(),  inverted.m33() };

    QRect inputArea = iInputArea;
    QRect outputArea = iOutputArea;
    inputArea.moveTopLeft( iOrigin );
    QPolygonF           outputRect = MapToPolygonF( iTransfo, inputArea );
    QRect transfoBBox = ExclusiveBoundingBox( outputRect );
    transfoBBox = transfoBBox.intersected( iOutputArea );

    // Get these before changong width/height
    const int rightLimit = transfoBBox.right();
    const int bottomLimit = transfoBBox.bottom();

    // 256 alignment
    transfoBBox.setWidth( (transfoBBox.width() | 255) + 1 );


    cl::Kernel* kernel = GetKernel( "TransformationNN" );

    kernel->setArg( 0, *iInput );
    kernel->setArg( 1, *oOutput );
    kernel->setArg( 2, iInputBPL );
    kernel->setArg( 3, iOutputBPL );

    kernel->setArg( 4, rightLimit );
    kernel->setArg( 5, bottomLimit );

    //kernel->setArg( 6, *mTransformationInverse );

    //kernel->setArg( 7, iOrigin.x() );
    //kernel->setArg( 8, iOrigin.y() );

    //kernel->setArg( 9, inputArea.left() );
    //kernel->setArg( 10, inputArea.right() );
    //kernel->setArg( 11, inputArea.top() );
    //kernel->setArg( 12, inputArea.bottom() );

    //mQueue->enqueueWriteBuffer( *mTransformationInverse, CL_TRUE, 0, sizeof(float)*9, transMatrixInverse );

    kernel->setArg( 6, iOrigin.x() );
    kernel->setArg( 7, iOrigin.y() );

    kernel->setArg( 8, inputArea.left() );
    kernel->setArg( 9, inputArea.right() );
    kernel->setArg( 10, inputArea.top() );
    kernel->setArg( 11, inputArea.bottom() );

    kernel->setArg( 12, transMatrixInverse[ 0 ] );
    kernel->setArg( 13, transMatrixInverse[ 1 ] );
    kernel->setArg( 14, transMatrixInverse[ 2 ] );
    kernel->setArg( 15, transMatrixInverse[ 3 ] );
    kernel->setArg( 16, transMatrixInverse[ 4 ] );
    kernel->setArg( 17, transMatrixInverse[ 5 ] );
    kernel->setArg( 18, transMatrixInverse[ 6 ] );
    kernel->setArg( 19, transMatrixInverse[ 7 ] );
    kernel->setArg( 20, transMatrixInverse[ 8 ] );

    cl_int error = mQueue->enqueueNDRangeKernel( *kernel, cl::NDRange( transfoBBox.left(), transfoBBox.top() ), cl::NDRange( transfoBBox.width(), transfoBBox.height() ), cl::NDRange( 256, 1 ) );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << error;
}


void
GPUForThisApp::_BlendImage( cl::Buffer* iInput, const QRect& iInputRect,  int iInputBPL,
                            cl::Buffer* oOutput,  const QRect& iOutputRect, int iOutputBPL,
                            const QPoint& point )
{
    // Drawing area
    const int minX = point.x();
    const int maxX = minX + iInputRect.width() - 1;
    const int minY = point.y();
    const int maxY = minY + iInputRect.height() - 1;

    const int startingX = minX < 0 ? 0 : minX;
    const int endingX = maxX >= iOutputRect.width() ? iOutputRect.width() - 1 : maxX;
    const int startingY = minY < 0 ? 0 : minY;
    const int endingY = maxY >= iOutputRect.height() ? iOutputRect.height() - 1 : maxY;

    QRect dstDrawingArea( startingX, startingY, endingX - startingX, endingY - startingY );
    dstDrawingArea.setWidth( (dstDrawingArea.width() | 255) + 1);

    cl::Kernel* normalBlend = GetKernel( "NormalBlendNotSameSizes" );

    normalBlend->setArg( 0, *iInput );
    normalBlend->setArg( 1, iInputBPL );
    normalBlend->setArg( 2, *oOutput );
    normalBlend->setArg( 3, iOutputBPL );
    normalBlend->setArg( 4, dstDrawingArea.left() );
    normalBlend->setArg( 5, dstDrawingArea.top() );
    normalBlend->setArg( 6, endingX +1 );
    normalBlend->setArg( 7, endingY +1 );

    cl_int error = mQueue->enqueueNDRangeKernel( *normalBlend, cl::NDRange( dstDrawingArea.left(), dstDrawingArea.top() ), cl::NDRange( dstDrawingArea.width(), dstDrawingArea.height() ), cl::NDRange( 256, 1 ) );
    if( error != CL_SUCCESS )
        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << error;
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
            qDebug() << "FAILED KERNEL : " << __FUNCTION__ << error;
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



// ====================================================
// ==============SELECTION TRANSFORMATION==============
// ====================================================



void
GPUForThisApp::LoadSelectionOriginalBuffer( QImage * iBuff )
{
    mInputImage = iBuff;
    DeleteBuffer( mSelInputBuffer );
    mSelInputBuffer = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_ONLY );
    mQueue->enqueueWriteBuffer( *mSelInputBuffer, CL_FALSE, 0, iBuff->sizeInBytes(), iBuff->bits() );
}


void
GPUForThisApp::LoadSelectionOutputImage( QImage * iBuff )
{
    mOutputImage = iBuff;
    DeleteBuffer( mSelOutputBuffer );
    mSelOutputBuffer = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_WRITE );
    mQueue->enqueueWriteBuffer( *mSelOutputBuffer, CL_FALSE, 0, iBuff->sizeInBytes(), iBuff->bits() );
}


void
GPUForThisApp::ClearSelectionBuffers()
{
    DeleteBuffer( mSelInputBuffer );
    mSelInputBuffer = 0;
    DeleteBuffer( mSelOutputBuffer );
    mSelOutputBuffer = 0;
}


void
GPUForThisApp::PerformTransformation( const QTransform& iTransfo, const QPoint& iOrigin )
{
    _TransformNN( mSelInputBuffer, mInputImage->bytesPerLine(), mInputImage->rect(), mSelOutputBuffer, mOutputImage->bytesPerLine(), mOutputImage->rect(), iTransfo, iOrigin );
    mQueue->finish();

    mQueue->enqueueReadBuffer( *mSelOutputBuffer, CL_TRUE, 0, mOutputImage->sizeInBytes(), mOutputImage->bits() );
}



void GPUForThisApp::LoadPaintContext( QImage * iBuff )
{
    mPaintContext = iBuff;
    DeleteBuffer( mBufferPaintContext );
    mBufferPaintContext = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_WRITE );
    mQueue->enqueueWriteBuffer( *mBufferPaintContext, CL_FALSE, 0, iBuff->sizeInBytes(), iBuff->bits() );
}


void
GPUForThisApp::LoadPaintAlpha( const QImage * iBuff )
{
    mPaintAlpha = iBuff;
    DeleteBuffer( mBufferPaintAlpha );
    mBufferPaintAlpha = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_ONLY );
    mQueue->enqueueWriteBuffer( *mBufferPaintAlpha, CL_FALSE, 0, iBuff->sizeInBytes(), iBuff->bits() );
}


void
GPUForThisApp::LoadBrushTip( const QImage * iBuff )
{
    mBrushTip = iBuff;
    DeleteBuffer( mBufferBrushTip );
    mBufferBrushTip = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_ONLY );
    mBufferBrushTipScaled = NewBuffer( iBuff->sizeInBytes(), CL_MEM_READ_WRITE );
    mQueue->enqueueWriteBuffer( *mBufferBrushTip, CL_FALSE, 0, iBuff->sizeInBytes(), iBuff->bits() );
}


void
GPUForThisApp::ClearPaintToolBuffers()
{
    DeleteBuffer( mBufferPaintContext );
    mBufferPaintContext = 0;
    DeleteBuffer( mBufferPaintAlpha );
    mBufferPaintAlpha = 0;
    DeleteBuffer( mBufferBrushTip );
    mBufferBrushTip = 0;
}


void
GPUForThisApp::Paint( int iX, int iY, float iPressure, float iRotatio )
{
    QTransform trans = QTransform::fromScale( iPressure, iPressure );
    QRect outputArea = trans.mapRect( mBrushTip->rect() );
    outputArea.moveCenter( QPoint( iX, iY ) );

    // This will write in mBufferBrushTipScaled the scaled buffer, in the area outputArea
    _TransformNN( mBufferBrushTip, mBrushTip->bytesPerLine(), mBrushTip->rect(),
                  mBufferBrushTipScaled, mBrushTip->bytesPerLine(), mBrushTip->rect(),
                  trans, QPoint( 0, 0 ) );

    mQueue->finish();

    _BlendImage( mBufferBrushTipScaled, outputArea, mBrushTip->bytesPerLine(),
                 mBufferPaintContext, mPaintContext->rect(), mPaintContext->bytesPerLine(),
                 outputArea.topLeft() );

    //mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );

    //bool useAlphaMask = mAlphaMask && mAlphaMask->width() == mDrawingContext->width() && mAlphaMask->height() == mDrawingContext->height();
    //if( useAlphaMask )
    //{
    //    alphaData = mAlphaMask->bits();
    //    alphaScanline = alphaData;
    //}

    //for( int y = startingY; y < endingY ; ++y )
    //{
    //    int offset = y * bytesPerLine + startingX * 4;
    //    dataScanline = data +  offset;
    //    dataTipScanline = dataTip + (y - minY) * bytesPerLineTip + (startingX - minX ) * 4;

    //    if( useAlphaMask )
    //        alphaScanline = alphaData +  offset + 3;

    //    for( int x = startingX; x < endingX; ++x )
    //    {
    //        int srcB = *dataTipScanline; ++dataTipScanline;
    //        int srcG = *dataTipScanline; ++dataTipScanline;
    //        int srcR = *dataTipScanline; ++dataTipScanline;
    //        int srcA = *dataTipScanline; ++dataTipScanline;

    //        if( useAlphaMask )
    //        {
    //            int alphaMaskTransparency = *alphaScanline; alphaScanline += 4;
    //            srcR = BlinnMult( srcR, alphaMaskTransparency );
    //            srcG = BlinnMult( srcG, alphaMaskTransparency );
    //            srcB = BlinnMult( srcB, alphaMaskTransparency );
    //            srcA = BlinnMult( srcA, alphaMaskTransparency );
    //        }

    //        if( srcA == 0 )
    //        {
    //            dataScanline += 4;
    //        }
    //        else
    //        {
    //            BlendPixelNormal( &dataScanline, srcR, srcG, srcB, srcA );
    //        }
    //    }
    //}

    //if( _mToolSizeAfterPressure != mToolSize )
    //    delete  scaledTip;

    mQueue->finish();

    cl::size_t<3> offset; // In byte too
    offset[ 0 ] = outputArea.left() * 4;
    offset[ 1 ] = outputArea.top();

    cl::size_t<3> size; // In bytes
    size[ 0 ] = outputArea.width()  *4; // Pixel is 4 bytes wide
    size[ 1 ] = outputArea.height() *1; // 1 byte high
    size[ 2 ] = 1; // 1 Byte deep

    qDebug() << "Reading : " << outputArea;

    mQueue->enqueueReadBufferRect( *mBufferPaintContext, CL_TRUE, offset, offset, size, mPaintContext->bytesPerLine(), 0, mPaintContext->bytesPerLine(), 0, mPaintContext->bits() );

}


void
GPUForThisApp::Bench( QImage* source, QImage* destination )
{
    //const int oneM = 1024000;
    //uchar* m1 = new uchar[ oneM ];

    //for( int i = 0; i < oneM; ++i )
    //    m1[ i ] = 1;

    //cl::Buffer oneMArray( *mContext, CL_MEM_READ_ONLY, sizeof( uchar ) * oneM );


    //qDebug() << "Few Big ============================================";
    //BENCHSTART( 1000 )
    //cl::Kernel* kernel = GetKernel( "Tests" );
    //kernel->setArg( 0, oneMArray );

    //mQueue->enqueueWriteBuffer( oneMArray, CL_FALSE, 0, sizeof( uchar ) * oneM, m1 );
    //cl_int err = mQueue->enqueueNDRangeKernel( *kernel, cl::NullRange, cl::NDRange( 512, 500 ), cl::NDRange( 512, 2 ) );
    //if( err != CL_SUCCESS )
    //    qDebug() << "FAILED KERNEL : " << __FUNCTION__ << err;

    //mQueue->finish();
    //BENCHEND( 1000 )


    //delete[]  m1;


    //const int small = 1024;
    //uchar m2[ small ];

    //for( int i = 0; i < small; ++i )
    //{
    //    m2[ i ] = 2;
    //}

    //cl::Buffer smallArray( *mContext, CL_MEM_READ_ONLY, sizeof( uchar ) * small );

    //qDebug() << "Lots small ============================================";
    //BENCHSTART( 100000 )
    //cl::Kernel* kernel2 = GetKernel( "Tests" );
    //kernel2->setArg( 0, smallArray );

    //mQueue->enqueueWriteBuffer( smallArray, CL_FALSE, 0, sizeof( uchar ) * small, m2 );
    //cl_int err = mQueue->enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 256, 4 ), cl::NDRange( 256, 4 ) );
    //if( err != CL_SUCCESS )
    //    qDebug() << "FAILED KERNEL : " << __FUNCTION__ << err;
    //mQueue->finish();

    //BENCHEND( 100000 )





    const int iter = 1000;


    const int oneM = source->sizeInBytes();
    const int BPL = source->bytesPerLine(); // Bytes per line

    cl::CommandQueue q1( *mContext, mDevice );
    cl::CommandQueue q2( *mContext, mDevice );
    cl::CommandQueue q3( *mContext, mDevice );
    cl::CommandQueue q4( *mContext, mDevice );
    cl::CommandQueue q5( *mContext, mDevice );
    cl::CommandQueue q6( *mContext, mDevice );
    cl::CommandQueue q7( *mContext, mDevice );
    cl::CommandQueue q8( *mContext, mDevice );

    //// Mocking an input image of size 32000*32
    //const int oneM = 419430400; // a 10240 x 10240 image
    //const int BPL = 40960; // Bytes per line
    //uchar* m1 = new uchar[ oneM ];

    //for( int i = 0; i < oneM; ++i )
    //    m1[ i ] = 1;

    cl::Buffer* image = NewBuffer( sizeof( uchar ) * source->sizeInBytes(), CL_MEM_READ_ONLY );
    mQueue->enqueueWriteBuffer( *image, CL_FALSE, 0, sizeof( uchar ) * oneM, source->bits() );

    // Mocking an output buffer for the gpu to write to
    cl::Buffer* imageOutput = NewBuffer( sizeof( uchar ) * oneM, CL_MEM_READ_WRITE );

    // Mocking a 3x3 transformation matrix
    const int nine = 9;
    float m2[ nine ];

    for( int i = 0; i < nine; ++i )
        m2[ i ] = 2.F;

    cl::Buffer transfo( *mContext, CL_MEM_READ_ONLY, sizeof( float ) * nine );


    ////==================================================================================
    ////==================================================================================
    ////==================================================================================
    ////==================================================================================


    ////// NO OPTIMIZATION :    0.38 -> no access
    //////                      0.38 -> 8 read access
    //////                      1.38 -> 4 Write access
    //////                      3.42 -> 4 Write access + 8 Read access

    ////std::chrono::high_resolution_clock::time_point  clock;
    ////clock = std::chrono::high_resolution_clock::now();
    ////for( int i = 0; i< iter; ++i )
    ////{

    ////    cl::Kernel* kernel2 = GetKernel( "Tests" );
    ////    kernel2->setArg( 0, *image );
    ////    kernel2->setArg( 1, *imageOutput );
    ////    kernel2->setArg( 2, BPL );
    ////    kernel2->setArg( 3, transfo );

    ////    mQueue->enqueueWriteBuffer( transfo, CL_FALSE, 0, sizeof( float ) * nine, m2 );
    ////    cl_int err = mQueue->enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 10240, 10240 ), cl::NDRange( 256, 1 ) );
    ////    if( err != CL_SUCCESS )
    ////        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << err;

    ////    mQueue->finish();

    ////}
    ////std::chrono::duration< double > timeSpan = std::chrono::duration_cast<std::chrono::duration< double >>( std::chrono::high_resolution_clock::now() - clock );
    ////double time = timeSpan.count();
    ////std::cout << "Time : Total : " << time << " Unit : " << time / iter;


    ////==================================================================================
    ////==================================================================================
    ////==================================================================================
    ////==================================================================================


    //// All access and writes are x4 because each kernel has to do 4 pixels
    //// GlobalSize / 4 :     0.11 -> no access
    ////                      0.11 -> 32 read access
    ////                      2.03 -> 16 Write access
    ////                      4.46 -> 16 Write access + 32 Read access ; 3.6 with local_size( 256, 1 )
    //// 4.2


    //// 2.7
    //// with possible coalescing access (no way to prove it's actually coalised ... but algo is supposed to)
    //// This is the best way so far, achieved by reading componentns side by side : 1 thread reads r value, the next reads the green, the next the blue etc...
    //// Then writing is done the same, the one who read red value writes red component etc...
    //// Each thread reads 4 components, so red, red +256, red +512, red +768, so that it's less thread to spawn


    //std::chrono::high_resolution_clock::time_point  clock;
    //clock = std::chrono::high_resolution_clock::now();
    //for( int i = 0; i< iter; ++i )
    //{

    //    cl::Kernel* kernel2 = GetKernel( "Tests" );

    //    kernel2->setArg( 0, *image );
    //    kernel2->setArg( 1, *imageOutput );
    //    kernel2->setArg( 2, BPL );
    //    kernel2->setArg( 3, transfo );

    //    mQueue->enqueueWriteBuffer( transfo, CL_FALSE, 0, sizeof( float ) * nine, m2 );
    //    cl_int err = mQueue->enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256,1 ) );
    //    if( err != CL_SUCCESS )
    //        qDebug() << "FAILED KERNEL : " << __FUNCTION__ << err;
    //    mQueue->finish();
    //}
    //std::chrono::duration< double > timeSpan = std::chrono::duration_cast<std::chrono::duration< double >>( std::chrono::high_resolution_clock::now() - clock );
    //double time = timeSpan.count();
    //std::cout << "Time : Total : " << time << " Unit : " << time / iter;


    ////==================================================================================
    ////==================================================================================
    ////==================================================================================
    ////==================================================================================


    // All access and writes are x4 because each kernel has to do 4 pixels
    // GlobalSize / 4 :     0.11 -> no access
    //                      0.11 -> 32 read access
    //                      2.03 -> 16 Write access
    //                      4.46 -> 16 Write access + 32 Read access
    cl::Kernel* kernel2 = GetKernel( "Tests" );
    kernel2->setArg( 0, *image );
    kernel2->setArg( 1, *imageOutput );
    kernel2->setArg( 2, BPL );

    std::chrono::high_resolution_clock::time_point  clock;
    clock = std::chrono::high_resolution_clock::now();
    for( int i = 0; i< iter/4; ++i )
    {

        kernel2->setArg( 3, transfo );

        mQueue->enqueueWriteBuffer( transfo, CL_FALSE, 0, sizeof( float ) * nine, m2 );

        //cl_int err = mQueue->enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );


        cl_int err = q1.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        err = q2.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        err = q3.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        err = q4.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        //err = q5.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        //err = q6.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        //err = q7.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );
        //err = q8.enqueueNDRangeKernel( *kernel2, cl::NullRange, cl::NDRange( 2048, 1080 ), cl::NDRange( 256, 1 ) );

        if( err != CL_SUCCESS )
            qDebug() << "FAILED KERNEL : " << __FUNCTION__ << err;

        //mQueue->finish();
    }

    q1.finish();
    q2.finish();
    q3.finish();
    q4.finish();

    std::chrono::duration< double > timeSpan = std::chrono::duration_cast<std::chrono::duration< double >>( std::chrono::high_resolution_clock::now() - clock );
    double time = timeSpan.count();
    std::cout << "Time : Total : " << time << " Unit : " << time / iter << std::endl;

    BENCHSTART( 1 )
    mQueue->enqueueReadBuffer( *imageOutput, CL_TRUE, 0, oneM, destination->bits() );
    BENCHEND( 1 )

    DeleteBuffer( image );
    DeleteBuffer( imageOutput );
}
