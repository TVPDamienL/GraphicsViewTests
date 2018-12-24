#pragma once

#include <vector>

class cLayer;

class cClip
{
public:
    virtual ~cClip();
    cClip( unsigned int iWidth, unsigned int iHeight );


    //Layers
public:
    cLayer* LayerAtIndex( int iIndex );
    void    AddLayer();


    //GetSet
public:
    unsigned int Width() const;
    unsigned int Height() const;


private:
    std::string             mName;
    unsigned int            mWidth;
    unsigned int            mHeight;

    std::vector< cLayer* >  mLayers;
};


