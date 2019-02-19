#pragma once

#include <QImage>
#include <CL/cl.hpp>

#include <iostream>

#include "BenchmarkStuff.h"



class GPUManager
{
public:
    ~GPUManager();
    GPUManager();


public:
    QImage*  GaussianBlurGPU1D( QImage* image );
    QImage*  GaussianBlurGPU1D2Pass( QImage* image );
    // Assumes source && dest have same size
    void  BlendImageSameSizesGPU( QImage* source, QImage* destination, const QRect& area, int iBlendingMode );
    void  FillImageGPU( QImage* destination, const QRect& area, const QColor& iColor, int iBlendingMode );
    QImage*  GaussianBlurCPP( QImage* image );
    QImage*  GaussianBlurCPPDoublePass( QImage* image );


private:
    cl::Platform    mPlatform;
    cl::Device      mDevice;
    cl::Context*    mContext;
};