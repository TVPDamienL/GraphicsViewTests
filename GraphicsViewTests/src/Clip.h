#pragma once

#include <vector>
#include <string>

#include "GPUFuncs.h"
#include <qpixmap.h>
#include "cSelection.h"
#include "cBaseData.h"

class cLayer;

class cClip :
    public cBaseData
{
public:
    enum eMessageClip : int
    {
        kDirty = 0,
        kRenderedDirtyArea,
    };

public:
    virtual ~cClip();
    cClip( unsigned int iWidth, unsigned int iHeight );


    void DirtyArea( const QRect& iArea );
    void DirtyAll();

    //Layers
public:
    cLayer* LayerAtIndex( int iIndex );
    cLayer* CurrentLayer();
    void    AddLayer();
    QImage* ComposeLayers();
    QImage* GetOutputImage();

    //GetSet
public:
    int Width() const;
    int Height() const;

public:
    cSelection* GetSelection();
    void        ExtractSelection();


private:
    std::string             mName;
    int                     mWidth;
    int                     mHeight;

    std::vector< cLayer* >  mLayers;
    cLayer*                 mCurrentLayer = 0;

    cSelection*             mSelection;
    QImage*                 mCurrentFrameRendering;
    QRect                   mDirtyArea = QRect( 0, 0, 0, 0 );

    GPUManager              mGPU;
};


