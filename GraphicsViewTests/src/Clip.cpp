#include "Clip.h"

#include "cLayer.h"

cClip::~cClip()
{
}


cClip::cClip( unsigned int iWidth, unsigned int iHeight ) :
    mName( "Layer" ),
    mWidth( iWidth ),
    mHeight( iHeight )
{
}


cLayer*
cClip::LayerAtIndex( int iIndex )
{
    return  mLayers[ iIndex ];;
}


void
cClip::AddLayer()
{
    mLayers.push_back( new cLayer( mWidth, mHeight ) );
}


unsigned int
cClip::Width() const
{
    return  mWidth;
}


unsigned int
cClip::Height() const
{
    return  mHeight;
}
