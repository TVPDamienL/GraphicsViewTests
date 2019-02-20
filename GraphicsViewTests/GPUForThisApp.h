#pragma once

#include "GPUController.h"

#include <QImage>


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


private:
    std::vector< cl::Buffer* > mLayers;
    cl::Buffer* mOutputBuffer;
    uint mLayerByteSize;
    uint mLayerBytePerLine; // Could differ from layer to layer
};