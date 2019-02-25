#pragma once

#include "GPUController.h"

#include <QImage>
#include <QRect>


#define _GPU GPUForThisApp::Instance()

class GPUForThisApp :
    public GPUController
{
public:
    ~GPUForThisApp();
    GPUForThisApp();

public:
    static GPUForThisApp* Instance();

public:
    QImage*  GaussianBlurGPU1D2Pass( QImage* image );

    // Assumes source && dest have same size
    void  BlendImageSameSizesGPU( QImage* source, QImage* destination, const QRect& area, int iBlendingMode );

    void  FillImageGPU( QImage* destination, const QRect& area, const QColor& iColor, int iBlendingMode );


    // GPU Version of standard methods
    void  _TransformNN( cl::Buffer* iInput, int iInputBPL, const QRect& iInputArea,
                        cl::Buffer* oOutput, int iOutputBPL, const QRect& iOutputArea,
                        const QTransform & iTransfo, const QPoint & iOrigin );

    void  _BlendImage( cl::Buffer* iInput, const QRect& iInputRect, int iInputBPL,
                       cl::Buffer* oOutput,  const QRect& iOutputRect, int iOutputBPL,
                       const QPoint& point );

    // Clip Compositing
    void  SetLayersByteSize( uint iSize );
    void  SetLayersBytePerLine( uint iSize );
    void  LoadLayerToGPU( QImage* iLayerImage );
    void  PerformLayerCompositing( QImage* oResult, const QRect& iDirtyArea );

    // Selection transformation
    void  LoadSelectionOriginalBuffer( QImage* iBuff );
    void  LoadSelectionOutputImage( QImage* iBuff );
    void  ClearSelectionBuffers();
    void  PerformTransformation( const QTransform& iTransfo, const QPoint& iOrigin );

    // Paint tool
    void  LoadPaintContext( QImage* iBuff );
    void  LoadPaintAlpha( const QImage* iBuff );
    void  LoadBrushTip( const QImage* iBuff );
    void  ClearPaintToolBuffers();
    void  Paint( int iX, int iY, float iPressure, float iRotatio );


private:
    // General purpose, kida dirty as it's allocation for the whole app life
    cl::Buffer* mTransformationInverse = 0;

    // Clip Compositing
    std::vector< cl::Buffer* > mLayers;
    cl::Buffer* mOutputBuffer;
    uint mLayerByteSize;
    uint mLayerBytePerLine; // Could differ from layer to layer


    // Selection transformation
    QImage*     mInputImage = 0;
    cl::Buffer* mSelInputBuffer = 0;
    QImage*     mOutputImage = 0;
    cl::Buffer* mSelOutputBuffer = 0;

    // PaintTool
    QImage*         mPaintContext;
    cl::Buffer*     mBufferPaintContext = 0;
    const QImage*   mPaintAlpha;
    cl::Buffer*     mBufferPaintAlpha = 0;
    const QImage*   mBrushTip;
    cl::Buffer*     mBufferBrushTip = 0;
    cl::Buffer*     mBufferBrushTipScaled = 0;
};