#pragma once


#include "cBaseData.h"
#include <QStandardItemModel>

#include <cmath>




// Pressure, azimut, angle, rotation etc...
struct sPointData
{
    QPoint mPosition;
    float  mPressure;
    float  mRotation;
};



class ToolBase :
    public QStandardItemModel,
    public cBaseData
{
    Q_OBJECT

public:
    ~ToolBase();
    ToolBase( QObject* iParent = Q_NULLPTR );



public:
    virtual  void   StartDrawing( QImage* iDC, sPointData iPointData ) = 0;
    virtual  QRect  MoveDrawing( sPointData iPointData ) = 0;
    virtual  QRect  EndDrawing( sPointData iPointData) = 0;

protected:
    QImage*     mDrawingContext = 0;
};






// EXTRACT



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
Distance2Points( const QPoint& iP1, const QPoint& iP2 )
{
    return  std::sqrt( Distance2PointsSquared( iP1, iP2 ) );
}



// EXTRACT


