#pragma once

#include <QImage>

#include "cConvolution.h"

#include "cBaseData.h"

class cClip;

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
    cSelection( int iWidth, int iHeight, cClip* iClip );

public:
    QImage* GetSelectionMask();
    QImage* GetSelectionContentImage();
    void    SetOriginalImage( QImage* iImage ) { mOriginalImage = iImage; }

    bool    IsActive() const;
    void    SetActive( bool iActive );

    void    EmitPainted();

public:
    void  Clear();
    void  ProcessEdgeDetection();

public:
    void ExtractPixelsFromImageToBuffer();
    void TransformSelection( const QTransform& iTransfo ); // Dirties the clip
    void CancelTransformation(); // Dirties the clip
    void ApplyTransformation(); // Dirties the clip


public:
    QImage*         GetSelectionEdgeMask();
    QRect           GetSelectionBBox() const;
    inline QRect    GetTransformationBBox() const
    {
        return  QRect( mTransformationBBox.left(),
                       mTransformationBBox.top(),
                       mTransformationBBox.width(),
                       mTransformationBBox.height() );
    }

public:
    QImage* ExtractedImage() { return  mExtractedBuffer; }
    QImage* TransformedImage() { return  mTransformationBuffer; }

private:
    void _FilterAlpha();

private:
    cClip*          mAssociatedClip = 0;
    QImage*         mMaskImage = 0;
    QImage*         mEdgeDetectedMaskImage = 0;
    QRect           mOriginalSelectionBBox;

    bool            mActive = false;
    QImage*         mOriginalImage = 0;         // The original image the selection is working on
    QImage*         mExtractedBuffer = 0;        // The extracted part
    QImage*         mExtratedBufferTMP = 0;        //TMP
    QImage*         mTransformationBuffer = 0;  // The extracted part transformed
    cConvolution    mEdgeDetectionConvolution;

    // Transformations
    QRectF         mTransformationBBox;
};

