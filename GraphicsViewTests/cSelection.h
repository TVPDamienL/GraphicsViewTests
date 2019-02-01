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

    bool    IsActive() const;
    void    SetActive( bool iActive );

    void    EmitPainted();

public:
    void  Clear();
    void  ProcessEdgeDetection();

public:
    void ExtractPixelsFromImageToBuffer( QImage* iImage );
    void TransformSelection( const QTransform& iTransfo, double iXScale, double iYScale ); // Dirties the clip
    void CancelTransformation(); // Dirties the clip
    void ApplyTransformation();


public:
    QImage* GetSelectionEdgeMask();
    QRect   GetSelectionBBox() const;
    inline QRect   GetTransformationBBox() const
    {
        return  QRect( mOriginalSelectionBBox.left() + mTransfoOffset.x(),
                       mOriginalSelectionBBox.top() + mTransfoOffset.y(),
                       mTransfoWidth,
                       mTransfoHeight );
    }

public:
    QImage* ExtractedImage() { return  mExtratedBuffer; }
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
    QImage*         mExtratedBuffer = 0;        // The extracted part
    QImage*         mTransformationBuffer = 0;  // The extracted part transformed
    cConvolution    mEdgeDetectionConvolution;

    // Transformations
    QPointF         mTransfoOffset = QPoint( 0, 0 );
    double          mTransfoWidth = 0.0;
    double          mTransfoHeight = 0.0;
    double          mTransfoRotation = 0.0;
};

