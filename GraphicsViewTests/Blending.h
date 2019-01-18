#pragma once

#include "Math.Fast.h"

#include <QtCore>

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



