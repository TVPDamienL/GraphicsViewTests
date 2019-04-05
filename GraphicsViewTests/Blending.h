#pragma once

#include "Math.Fast.h"

#include <QtCore>




// Raw pixel transfert
static
inline
void
BlendPixelNone( uchar** pixelDestination, quint8 r, quint8 g, quint8 b, quint8 a )
{
    **pixelDestination = b; ++*pixelDestination;
    **pixelDestination = g; ++*pixelDestination;
    **pixelDestination = r; ++*pixelDestination;
    **pixelDestination = a; ++*pixelDestination;
}



// Standard alpha blending
static
inline
void
BlendPixelNormal( uchar** pixelDestination, quint8 r, quint8 g, quint8 b, quint8 a )
{
    int transparencyAmountInverse = 255 - a;

    **pixelDestination = b + BlinnMult( **pixelDestination, transparencyAmountInverse ); ++*pixelDestination;
    **pixelDestination = g + BlinnMult( **pixelDestination, transparencyAmountInverse ); ++*pixelDestination;
    **pixelDestination = r + BlinnMult( **pixelDestination, transparencyAmountInverse ); ++*pixelDestination;
    **pixelDestination = a + BlinnMult( **pixelDestination, transparencyAmountInverse ); ++*pixelDestination;
}




// ========================
// Float Versions
// ========================




// Raw pixel transfert
static
inline
void
BlendPixelNoneF( float** pixelDestination, float r, float g, float b, float a )
{
    **pixelDestination = b; ++*pixelDestination;
    **pixelDestination = g; ++*pixelDestination;
    **pixelDestination = r; ++*pixelDestination;
    **pixelDestination = a; ++*pixelDestination;
}



// Standard alpha blending
static
inline
void
BlendPixelNormalF( float** pixelDestination, float r, float g, float b, float a )
{
    float transparencyAmountInverse = (255.F - a) / 255.F;

    **pixelDestination = b + **pixelDestination * transparencyAmountInverse; ++*pixelDestination;
    **pixelDestination = g + **pixelDestination * transparencyAmountInverse; ++*pixelDestination;
    **pixelDestination = r + **pixelDestination * transparencyAmountInverse; ++*pixelDestination;
    **pixelDestination = a + **pixelDestination * transparencyAmountInverse; ++*pixelDestination;
}






// Raw pixel transfert
static
inline
void
BlendPixelNoneFParrallel( float** pixelDestination, uchar** parallel, float r, float g, float b, float a )
{
    **pixelDestination = b; ++*pixelDestination;
    **parallel = uchar( b ); ++*parallel;
    **pixelDestination = g; ++*pixelDestination;
    **parallel = uchar( g ); ++*parallel;
    **pixelDestination = r; ++*pixelDestination;
    **parallel = uchar( r ); ++*parallel;
    **pixelDestination = a; ++*pixelDestination;
    **parallel = uchar( a ); ++*parallel;
}



// Standard alpha blending
static
inline
void
BlendPixelNormalFParrallel( float** pixelDestination, uchar** parallel, float r, float g, float b, float a )
{
    float transparencyAmountInverse = (255.F - a) / 255.F;

    **pixelDestination = b + **pixelDestination * transparencyAmountInverse;
    **parallel = uchar( **pixelDestination );
    ++*pixelDestination; ++*parallel;

    **pixelDestination = g + **pixelDestination * transparencyAmountInverse;
    **parallel = uchar( **pixelDestination );
    ++*pixelDestination; ++*parallel;

    **pixelDestination = r + **pixelDestination * transparencyAmountInverse;
    **parallel = uchar( **pixelDestination );
    ++*pixelDestination; ++*parallel;

    **pixelDestination = a + **pixelDestination * transparencyAmountInverse;
    **parallel = uchar( **pixelDestination );
    ++*pixelDestination; ++*parallel;
}




