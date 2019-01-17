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