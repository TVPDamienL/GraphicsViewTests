#pragma once

#include <QImage>

#include "cConvolution.h"

class cSelection
{
public:
    ~cSelection();
    cSelection( int iWidth, int iHeight );

public:
    QImage* GetSelectionMask();

    bool    IsActive() const;
    void    SetActive( bool iActive );

public:
    void  Clear();
    void  ProcessEdgeDetection();

public:
    void ExtractPixelsToBuffer( QImage* iImage );


public:
    QImage* GetSelectionEdgeMask();
    QRect   GetSelectionBBox() const;


private:
    void _FilterAlpha();

private:
    QImage*         mMaskImage;
    QImage*         mEdgeDetectedMaskImage;
    QImage*         mTransformBuffer;

    bool            mActive = false;
    QImage*         mSelectedBuffer;
    cConvolution    mEdgeDetectionConvolution;
};

