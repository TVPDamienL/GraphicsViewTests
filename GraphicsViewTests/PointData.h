#pragma once


#include <QPoint>

// Pressure, azimut, angle, rotation etc...
struct sPointData
{
    QPointF mPosition;
    float   mPressure;
    float   mRotation;

    bool operator ==( const sPointData& rhs )
    {
        return  mPosition == rhs.mPosition && mPressure == rhs.mPressure && mRotation == rhs.mRotation;
    }
};
