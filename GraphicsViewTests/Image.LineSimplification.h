#pragma once



#include "PaintToolBase.h"
#include "Math.Fast.h"



std::vector< sPointData >
SimplifyLine( const std::vector< sPointData >& InputLine, float iCurvatureThreshold, float iPressureThreshold )
{
    std::vector< sPointData > output = InputLine;
    if( InputLine.size() < 3 )
        return  output;

    for( int i = 1; i < output.size() - 1; ++i )
    {
        auto p1 = output[ i - 1 ];
        auto p2 = output[ i     ];
        auto p3 = output[ i + 1 ];

        float pressureDiffLeft = std::abs( p1.mPressure - p2.mPressure );
        float pressureDiffRight = std::abs( p2.mPressure - p3.mPressure );

        float d1 = Distance2Points( p1.mPosition, p2.mPosition );
        float d2 = Distance2Points( p2.mPosition, p3.mPosition );
        float d3 = Distance2Points( p3.mPosition, p1.mPosition );

        float halfPerimeter = ( d1 + d2 + d3 ) / 2;
        float areaSquared = halfPerimeter * ( halfPerimeter - d1 ) * halfPerimeter * ( halfPerimeter - d2 ) * halfPerimeter * ( halfPerimeter - d3 );

        if( !(( areaSquared > iCurvatureThreshold ) || ( pressureDiffLeft > iPressureThreshold ||  pressureDiffRight > iPressureThreshold )) )
        {
            output.erase( output.begin() + i );
            --i;
        }
    }
    return  output;
}

