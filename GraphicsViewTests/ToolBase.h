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

    bool operator ==( const sPointData& rhs )
    {
        return  mPosition == rhs.mPosition && mPressure == rhs.mPressure && mRotation == rhs.mRotation;
    }
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

    bool            Vectorial() const { return  mVectorial; }
    void            Vectorial( bool n ) { mVectorial = n; }



public:
    virtual  void   StartDrawing( QImage* iDC, sPointData iPointData ) = 0;
    virtual  QRect  MoveDrawing( sPointData iPointData ) = 0;
    virtual  QRect  EndDrawing( sPointData iPointData) = 0;

protected:
    QImage*     mDrawingContext = 0;
    bool        mVectorial = false;
};


