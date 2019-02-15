#pragma once


#include <cstdint>


/** This computes (a * b / 255).
Many thanks to Jim Blinn !
*/
inline uint8_t
BlinnMult( uint8_t iA, uint8_t iB )
{
    uint16_t  i = uint16_t( iA * iB + 128 );
    return  uint8_t((i + (i>>8)) >> 8);
}


static
inline
float
Distance2PointsSquared( const QPoint& iP1, const QPoint& iP2 )
{
    float distanceX = iP1.x() - iP2.x();
    float distanceY = iP1.y() - iP2.y();

    return  distanceX * distanceX + distanceY * distanceY;
}


static
inline
float
Distance2PointsSquared( const QPointF& iP1, const QPointF& iP2 )
{
    float distanceX = iP1.x() - iP2.x();
    float distanceY = iP1.y() - iP2.y();

    return  distanceX * distanceX + distanceY * distanceY;
}


static
inline
float
Distance2Points( const QPoint& iP1, const QPoint& iP2 )
{
    return  std::sqrt( Distance2PointsSquared( iP1, iP2 ) );
}


static
inline
float
Distance2Points( const QPointF& iP1, const QPointF& iP2 )
{
    return  std::sqrt( Distance2PointsSquared( iP1, iP2 ) );
}


template< typename T >
static
inline
T Min( const T& a, const T& b )
{
    if( a < b )
        return  a;

    return  b;
}


template< typename T >
static
inline
T Max( const T& a, const T& b )
{
    if( a > b )
        return  a;

    return  b;
}

