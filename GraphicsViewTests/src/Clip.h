#pragma once

#include <vector>
#include <string>

#include <qpixmap.h>
#include "cSelection.h"

class cLayer;

class cClip
{
public:
    virtual ~cClip();
    cClip( unsigned int iWidth, unsigned int iHeight );


    void DirtyArea( const QRect& iArea );

    //Layers
public:
    cLayer* LayerAtIndex( int iIndex );
    void    AddLayer();
    QImage* ComposeLayers();
    QImage* GetOutputImage();

    //GetSet
public:
    unsigned int Width() const;
    unsigned int Height() const;

public:
    cSelection* GetSelection();


private:
    std::string             mName;
    unsigned int            mWidth;
    unsigned int            mHeight;

    std::vector< cLayer* >  mLayers;

    cSelection*             mSelection;
    QImage*                 mCurrentFrameRendering;
    QRect                   mDirtyArea;
};


