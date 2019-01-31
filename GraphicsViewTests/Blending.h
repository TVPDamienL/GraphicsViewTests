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



