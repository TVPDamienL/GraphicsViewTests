#pragma once

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
    public QStandardItemModel
{
    Q_OBJECT

public:
    ToolBase(QObject *parent);
    ~ToolBase();


public:
    int             getSize() const;
    void            setSize( int iSize );

    QColor          getColor() const;
    void            setColor( const QColor& iColor );

    float           getStep() const;
    void            setStep( float iStep );


public:
    virtual  void   StartDrawing( QImage* iDC );

    virtual  QRect  MoveDrawing( sPointData iPointData );

    virtual  void   DrawDot( int x, int y, float iPressure, float iRotation ) = 0;
    virtual  void   DrawLine( int x1, int y1, int x2, int y2 ) = 0;
    virtual  void   DrawFullPath();
    virtual  void   DrawPathFromLastRenderedPoint();

    virtual  QRect  EndDrawing();

    void            PathAddPoint( sPointData iPoint );
    QRect           GetDirtyArea() const;
    QRect           GetDirtyAreaAndReset();


public:
    void  SetAlphaMask( QImage* iImage );
    void  ClearAlphaMask();

private:
    QPoint  __DrawDotVectorTruc_RequiresAName_( const QPoint& iStart, const QPointF& iVector, float iPressure, float iRotation );


protected:
    int                         mToolSize;
    QColor                      mColor;
    std::vector< sPointData >   mPath;
    float                       mStep;

    const QImage*               mAlphaMask;
    QRect                       mDirtyArea;

    QImage*                     mDrawingContext = 0;

private:
    int                         mLastRenderedPathIndex;
    float                       mRequiredStepLength;
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


