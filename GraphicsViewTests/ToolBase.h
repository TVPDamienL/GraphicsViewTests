#pragma once

#include <QStandardItemModel>

#include <cmath>

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

    float           getStep() const;
    void            setStep( float iStep );


public:
    virtual  void  StartDrawing();

    virtual  void  DrawDot( QImage* iImage, int x, int y ) = 0;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) = 0;
    virtual  void  DrawFullPath( QImage* iImage );
    virtual  void  DrawPathFromLastRenderedPoint( QImage* iImage );

    virtual  void  EndDrawing();


    void  PathAddPoint( const QPoint& iPoint );

private:
    QPoint  __DrawDotVectorTruc_RequiresAName_( QImage* iImage, const QPoint& iStart, const QPointF& iVector );




protected:
    int                     mToolSize;
    std::vector< QPoint >   mPath;
    float                   mStep;

private:
    int     mLastRenderedPathIndex;
    float   mRequiredPathLength;
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


