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
    QImage*         DrawingContext();


public:
    virtual  void   StartDrawing( QImage* iDC, sPointData iPointData ) = 0;
    virtual  QRect  MoveDrawing( sPointData iPointData ) = 0;
    virtual  QRect  EndDrawing( sPointData iPointData) = 0;

protected:
    QImage*     mDrawingContext = 0;
};


