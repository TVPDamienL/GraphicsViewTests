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


private:
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
};