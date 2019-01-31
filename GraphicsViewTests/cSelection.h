#pragma once

#include <QImage>

#include "cConvolution.h"

#include "cBaseData.h"

class cSelection :
    public cBaseData
{
public:
    enum eMessageSelection : int
    {
        kActiveChanged = 0,
        kBoundsChanged,
        kPainted,
        kDEBUG,
    };

public:
    ~cSelection();
    cSelection( int iWidth, int iHeight );

public:
    QImage* GetSelectionMask();
    QImage* GetSelectionContentImage();

    bool    IsActive() const;
    void    SetActive( bool iActive );

    void    EmitPainted();

public:
    void  Clear();
    void  ProcessEdgeDetection();

public:
    void ExtractPixelsFromImageToBuffer( QImage* iImage );
    void TransformSelection( const QTransform& iTransfo, double iXScale, double iYScale );
    void CancelTransformation();
    void ApplyTransformation();


public:
    QImage* GetSelectionEdgeMask();
    QRect   GetSelectionBBox() const;

public:
    QImage* ExtractedImage() { return  mExtratedBuffer; }
    QImage* TransformedImage() { return  mTransformationBuffer; }

private:
    void _FilterAlpha();

private:
    QImage*         mMaskImage = 0;
    QImage*         mEdgeDetectedMaskImage = 0;
    QRect           mSelectionBBox;

    bool            mActive = false;
    QImage*         mOriginalImage = 0;         // The original image the selection is working on
    QImage*         mExtratedBuffer = 0;        // The extracted part
    QImage*         mTransformationBuffer = 0;  // The extracted part transformed
    cConvolution    mEdgeDetectionConvolution;
};

