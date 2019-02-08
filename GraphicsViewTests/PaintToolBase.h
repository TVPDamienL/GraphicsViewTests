#pragma once

#include "ToolBase.h"

class cPaintToolBase :
    public  ToolBase
{
    Q_OBJECT

public:
    ~cPaintToolBase();
    cPaintToolBase( QObject* iParent = Q_NULLPTR );

public:
    int             getSize() const;
    void            setSize( int iSize );

    QColor          getColor() const;
    void            setColor( const QColor& iColor );

    float           getStep() const;
    void            setStep( float iStep );

public:
    virtual  void   StartDrawing( QImage* iDC, sPointData iPointData ) override;
    virtual  QRect  MoveDrawing( sPointData iPointData ) override;
    virtual  QRect  EndDrawing( sPointData iPointData ) override;

public:
    virtual  void   DrawDot( int x, int y, float iPressure, float iRotation ) = 0;
    virtual  void   DrawLine( int x1, int y1, int x2, int y2 ) = 0;
    virtual  void   DrawFullPath();
    virtual  void   DrawPathFromLastRenderedPoint();

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

private:
    int                         mLastRenderedPathIndex;
    float                       mRequiredStepLength;

};