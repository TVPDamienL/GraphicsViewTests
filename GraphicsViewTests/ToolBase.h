#pragma once

#include <QStandardItemModel>



class ToolBase :
    public QStandardItemModel
{
    Q_OBJECT

public:
    ToolBase(QObject *parent);
    ~ToolBase();

public:
    virtual  void  DrawDot( QImage* iImage, int x, int y ) = 0;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) = 0;
    virtual  void  DrawPath( QImage* iImage ) = 0;

    void  PathAddPoint( const QPoint& iPoint );

protected:
    std::vector< QPoint > mPath;
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
    return  sqrt( Distance2PointsSquared( iP1, iP2 ) );
}



// EXTRACT


